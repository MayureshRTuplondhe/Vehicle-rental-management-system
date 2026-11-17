#include "http_server.hpp"

#include "booking_service.hpp"
#include "database.hpp"
#include "payment_worker.hpp"
#include "rental_error.hpp"

#include <winsock2.h>
#include <ws2tcpip.h>

#include <cctype>
#include <csignal>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <map>
#include <random>
#include <sstream>
#include <string>
#include <vector>

namespace {
struct HttpRequest {
  std::string method;
  std::string target;
  std::string path;
  std::string body;
  std::map<std::string, std::string> headers;
};
struct Session {
  int id;
  std::string role;
};
std::map<std::string, Session> sessions;
volatile std::sig_atomic_t serverRunning = 1;

void handleInterrupt(int) { serverRunning = 0; }

std::string lower(std::string value) {
  for (std::string::iterator it = value.begin(); it != value.end(); ++it)
    *it = static_cast<char>(std::tolower(static_cast<unsigned char>(*it)));
  return value;
}

std::string decode(const std::string &value) {
  std::string out;
  for (std::size_t i = 0; i < value.size(); ++i) {
    if (value[i] == '+')
      out.push_back(' ');
    else if (value[i] == '%' && i + 2 < value.size()) {
      const std::string hex = value.substr(i + 1, 2);
      out.push_back(static_cast<char>(std::strtol(hex.c_str(), NULL, 16)));
      i += 2;
    } else
      out.push_back(value[i]);
  }
  return out;
}

std::map<std::string, std::string> parameters(const std::string &data) {
  std::map<std::string, std::string> values;
  std::size_t begin = 0;
  while (begin <= data.size()) {
    const std::size_t end = data.find('&', begin);
    const std::string field =
        data.substr(begin, end == std::string::npos ? end : end - begin);
    const std::size_t equals = field.find('=');
    if (equals != std::string::npos)
      values[decode(field.substr(0, equals))] =
          decode(field.substr(equals + 1));
    if (end == std::string::npos)
      break;
    begin = end + 1;
  }
  return values;
}

std::string quote(const std::string &value) {
  std::ostringstream out;
  out << '"';
  for (std::size_t i = 0; i < value.size(); ++i) {
    const unsigned char c = static_cast<unsigned char>(value[i]);
    if (c == '"' || c == '\\')
      out << '\\' << c;
    else if (c == '\n')
      out << "\\n";
    else if (c == '\r')
      out << "\\r";
    else if (c == '\t')
      out << "\\t";
    else if (c < 32)
      out << ' ';
    else
      out << c;
  }
  out << '"';
  return out.str();
}

std::string formValue(const std::map<std::string, std::string> &values,
                      const std::string &key) {
  std::map<std::string, std::string>::const_iterator found = values.find(key);
  return found == values.end() ? "" : found->second;
}

std::string makeToken() {
  static std::mt19937 generator(
      static_cast<unsigned int>(std::random_device()()));
  static const char digits[] = "0123456789abcdef";
  std::string token;
  for (int i = 0; i < 32; ++i)
    token.push_back(digits[generator() % 16]);
  return token;
}

bool readRequest(SOCKET client, HttpRequest &request) {
  std::string data;
  char buffer[4096];
  std::size_t headerEnd = std::string::npos;
  std::size_t contentLength = 0;
  while (headerEnd == std::string::npos ||
         data.size() < headerEnd + 4 + contentLength) {
    const int received = recv(client, buffer, sizeof(buffer), 0);
    if (received <= 0)
      return false;
    data.append(buffer, received);
    if (data.size() > 1024 * 1024)
      return false;
    headerEnd = data.find("\r\n\r\n");
    if (headerEnd != std::string::npos && contentLength == 0) {
      const std::string headers = data.substr(0, headerEnd);
      std::istringstream lines(headers);
      std::string line;
      std::getline(lines, line);
      std::istringstream first(line);
      first >> request.method >> request.target;
      while (std::getline(lines, line)) {
        if (!line.empty() && line[line.size() - 1] == '\r')
          line.erase(line.size() - 1);
        const std::size_t colon = line.find(':');
        if (colon != std::string::npos) {
          const std::string key = lower(line.substr(0, colon));
          std::string value = line.substr(colon + 1);
          while (!value.empty() && value[0] == ' ')
            value.erase(0, 1);
          request.headers[key] = value;
          if (key == "content-length")
            contentLength =
                static_cast<std::size_t>(std::strtoul(value.c_str(), NULL, 10));
        }
      }
    }
  }
  const std::size_t question = request.target.find('?');
  request.path = request.target.substr(0, question);
  if (headerEnd != std::string::npos)
    request.body = data.substr(headerEnd + 4, contentLength);
  return true;
}

void sendResponse(
    SOCKET client, int status, const std::string &body,
    const std::string &contentType = "application/json; charset=utf-8") {
  const char *reason = status == 200   ? "OK"
                       : status == 201 ? "Created"
                       : status == 400 ? "Bad Request"
                       : status == 401 ? "Unauthorized"
                       : status == 403 ? "Forbidden"
                       : status == 404 ? "Not Found"
                                       : "Internal Server Error";
  std::ostringstream response;
  response << "HTTP/1.1 " << status << ' ' << reason << "\r\n"
           << "Content-Type: " << contentType << "\r\n"
           << "Content-Length: " << body.size() << "\r\n"
           << "Connection: close\r\n"
           << "Access-Control-Allow-Origin: *\r\n"
           << "Access-Control-Allow-Headers: Authorization, Content-Type\r\n"
           << "Access-Control-Allow-Methods: GET, POST, DELETE, OPTIONS\r\n\r\n"
           << body;
  const std::string text = response.str();
  std::size_t sent = 0;
  while (sent < text.size()) {
    const int result = send(client, text.data() + sent,
                            static_cast<int>(text.size() - sent), 0);
    if (result <= 0)
      break;
    sent += static_cast<std::size_t>(result);
  }
}

void jsonResponse(SOCKET client, int status, const std::string &json) {
  sendResponse(client, status, json);
}

void fail(SOCKET client, int status, const std::string &message) {
  jsonResponse(client, status, "{\"error\":" + quote(message) + "}");
}

bool authorize(const HttpRequest &request, SOCKET client, Session &session,
               const std::string &requiredRole = "") {
  std::map<std::string, std::string>::const_iterator auth =
      request.headers.find("authorization");
  if (auth == request.headers.end() ||
      auth->second.compare(0, 7, "Bearer ") != 0) {
    fail(client, 401, "Please log in first.");
    return false;
  }
  const std::string token = auth->second.substr(7);
  std::map<std::string, Session>::const_iterator found = sessions.find(token);
  if (found == sessions.end()) {
    fail(client, 401, "Your session expired. Please log in again.");
    return false;
  }
  session = found->second;
  if (!requiredRole.empty() && session.role != requiredRole) {
    fail(client, 403, "Your account cannot perform this action.");
    return false;
  }
  return true;
}

std::string userJson(int id, const std::string &name, const std::string &email,
                     const std::string &phone, const std::string &role,
                     const std::string &license) {
  return "{\"id\":" + std::to_string(id) + ",\"name\":" + quote(name) +
         ",\"email\":" + quote(email) + ",\"phone\":" + quote(phone) +
         ",\"role\":" + quote(lower(role)) + ",\"license\":" + quote(license) +
         "}";
}

void createAdmin() {
  std::string name, email, phone, password;
  std::cout << "Admin name: ";
  std::getline(std::cin, name);
  std::cout << "Admin email: ";
  std::getline(std::cin, email);
  std::cout << "Phone: ";
  std::getline(std::cin, phone);
  std::cout << "Password (local demo only): ";
  std::getline(std::cin, password);
  if (name.empty() || email.empty() || phone.empty() || password.empty())
    throw RentalError("Admin fields cannot be empty.");
  Statement insert(Database::instance().handle(),
                   "INSERT INTO users(name,email,phone,password_text,role) "
                   "VALUES(?,?,?,?, 'ADMIN')");
  insert.bind(1, name);
  insert.bind(2, lower(email));
  insert.bind(3, phone);
  insert.bind(4, password);
  insert.step();
  std::cout
      << "Admin account created. Start the server without --create-admin.\n";
}

void handle(SOCKET client, const HttpRequest &request,
            PaymentWorker &paymentWorker) {
  if (request.method == "OPTIONS") {
    sendResponse(client, 200, "", "text/plain");
    return;
  }
  if (request.method == "GET" && request.path == "/") {
    std::ifstream file("rentride (2).html", std::ios::binary);
    if (!file) {
      sendResponse(client, 500,
                   "Could not find rentride (2).html; run from Project/.",
                   "text/plain");
      return;
    }
    std::ostringstream html;
    html << file.rdbuf();
    sendResponse(client, 200, html.str(), "text/html; charset=utf-8");
    return;
  }

  if (request.method == "GET" && request.path == "/app.js") {
    std::ifstream file("app.js", std::ios::binary);
    if (!file) {
      sendResponse(client, 404, "Could not find app.js.", "text/plain");
      return;
    }
    std::ostringstream script;
    script << file.rdbuf();
    sendResponse(client, 200, script.str(),
                 "application/javascript; charset=utf-8");
    return;
  }

  const std::map<std::string, std::string> values = parameters(request.body);
  const std::size_t question = request.target.find('?');
  const std::map<std::string, std::string> query = parameters(
      question == std::string::npos ? "" : request.target.substr(question + 1));

  if (request.method == "GET" && request.path == "/api/vehicles") {
    const std::string stateFilter = formValue(query, "state");
    const char *sql =
        "SELECT "
        "v.id,v.owner_id,v.type,v.category,v.name,v.reg_number,v.fuel,v.seats,"
        "v.rate_per_day,"
        "CASE WHEN v.status='MAINTENANCE' THEN 'MAINTENANCE' WHEN "
        "EXISTS(SELECT 1 FROM bookings b WHERE b.vehicle_id=v.id AND "
        "b.status='ACTIVE' AND date('now') BETWEEN date(b.start_date) AND "
        "date(b.end_date)) THEN 'RENTED' ELSE 'AVAILABLE' END "
        "FROM vehicles v WHERE v.is_active=1 ORDER BY v.id";
    Statement result(Database::instance().handle(), sql);
    std::string out = "{\"vehicles\":[";
    bool first = true;
    while (result.step()) {
      const std::string status = result.text(9);
      if (stateFilter == "available" && status != "AVAILABLE")
        continue;
      if (stateFilter == "booked" && status != "RENTED")
        continue;
      if (!first)
        out += ',';
      first = false;
      out += "{\"id\":" + std::to_string(result.integer(0)) +
             ",\"ownerId\":" + std::to_string(result.integer(1)) +
             ",\"dbType\":" + quote(result.text(2)) +
             ",\"type\":" + quote(result.text(3)) +
             ",\"name\":" + quote(result.text(4)) +
             ",\"registration\":" + quote(result.text(5)) +
             ",\"fuel\":" + quote(result.text(6)) +
             ",\"seats\":" + std::to_string(result.integer(7)) +
             ",\"price\":" + std::to_string(result.real(8)) +
             ",\"status\":" + quote(status) + ",\"available\":" +
             std::string(status == "AVAILABLE" ? "true" : "false") + "}";
    }
    jsonResponse(client, 200, out + "]}");
    return;
  }

  if (request.method == "POST" && request.path == "/api/register") {
    const std::string role =
        lower(formValue(values, "role")) == "renter" ? "RENTER" : "CUSTOMER";
    const std::string name = formValue(values, "name"),
                      email = lower(formValue(values, "email"));
    const std::string phone = formValue(values, "phone"),
                      password = formValue(values, "password");
    const std::string license = formValue(values, "license");
    if (name.empty() || email.empty() || phone.empty() || password.empty() ||
        (role == "CUSTOMER" && license.empty())) {
      fail(client, 400, "Complete all required account fields.");
      return;
    }
    if (password.size() < 4) {
      fail(client, 400, "Password must contain at least 4 characters.");
      return;
    }
    Statement insert(
        Database::instance().handle(),
        "INSERT INTO users(name,email,phone,password_text,role,license_no) "
        "VALUES(?,?,?,?,?,?)");
    insert.bind(1, name);
    insert.bind(2, email);
    insert.bind(3, phone);
    insert.bind(4, password);
    insert.bind(5, role);
    if (license.empty())
      insert.bindNull(6);
    else
      insert.bind(6, license);
    insert.step();
    jsonResponse(client, 201,
                 "{\"ok\":true,\"id\":" +
                     std::to_string(sqlite3_last_insert_rowid(
                         Database::instance().handle())) +
                     "}");
    return;
  }

  if (request.method == "POST" && request.path == "/api/login") {
    const std::string identifier = formValue(values, "identifier"),
                      password = formValue(values, "password");
    Statement login(
        Database::instance().handle(),
        "SELECT id,name,email,phone,role,license_no,password_text FROM users "
        "WHERE lower(email)=lower(?) OR id=? ORDER BY id LIMIT 1");
    login.bind(1, identifier);
    char *end = NULL;
    const long numericId = std::strtol(identifier.c_str(), &end, 10);
    login.bind(2, end != identifier.c_str() && *end == '\0'
                      ? static_cast<int>(numericId)
                      : -1);
    if (!login.step() || login.text(6) != password) {
      fail(client, 401, "Email/user ID or password did not match.");
      return;
    }
    Session session;
    session.id = login.integer(0);
    session.role = login.text(4);
    const std::string token = makeToken();
    sessions[token] = session;
    jsonResponse(client, 200,
                 "{\"token\":" + quote(token) + ",\"user\":" +
                     userJson(session.id, login.text(1), login.text(2),
                              login.text(3), session.role, login.text(5)) +
                     "}");
    return;
  }

  if (request.method == "POST" && request.path == "/api/logout") {
    const std::string auth =
        request.headers.find("authorization") == request.headers.end()
            ? ""
            : request.headers.find("authorization")->second;
    if (auth.compare(0, 7, "Bearer ") == 0)
      sessions.erase(auth.substr(7));
    jsonResponse(client, 200, "{\"ok\":true}");
    return;
  }

  if (request.method == "GET" && request.path == "/api/me") {
    Session session;
    if (!authorize(request, client, session))
      return;
    Statement user(
        Database::instance().handle(),
        "SELECT name,email,phone,role,license_no FROM users WHERE id=?");
    user.bind(1, session.id);
    if (!user.step()) {
      fail(client, 401, "Account no longer exists.");
      return;
    }
    jsonResponse(client, 200,
                 "{\"user\":" +
                     userJson(session.id, user.text(0), user.text(1),
                              user.text(2), user.text(3), user.text(4)) +
                     "}");
    return;
  }

  if (request.method == "POST" && request.path == "/api/vehicles") {
    Session session;
    if (!authorize(request, client, session, "RENTER"))
      return;
    const std::string category = formValue(values, "category"),
                      name = formValue(values, "name");
    const std::string registration = formValue(values, "registration"),
                      fuel = formValue(values, "fuel");
    const int price = std::atoi(formValue(values, "price").c_str());
    const int seats = std::atoi(formValue(values, "seats").c_str());
    const std::string type =
        category == "Bike"                                    ? "BIKE"
        : (category == "Truck" || category == "Pickup Truck") ? "TRUCK"
                                                              : "CAR";
    if (category.empty() || name.empty() || registration.empty() ||
        price <= 0 || seats <= 0) {
      fail(client, 400, "Enter valid vehicle details.");
      return;
    }
    Statement insert(Database::instance().handle(),
                     "INSERT INTO "
                     "vehicles(owner_id,type,category,name,reg_number,fuel,"
                     "seats,rate_per_day) VALUES(?,?,?,?,?,?,?,?)");
    insert.bind(1, session.id);
    insert.bind(2, type);
    insert.bind(3, category);
    insert.bind(4, name);
    insert.bind(5, registration);
    insert.bind(6, fuel.empty() ? "Petrol" : fuel);
    insert.bind(7, seats);
    insert.bind(8, price);
    insert.step();
    jsonResponse(client, 201,
                 "{\"ok\":true,\"id\":" +
                     std::to_string(sqlite3_last_insert_rowid(
                         Database::instance().handle())) +
                     "}");
    return;
  }

  if (request.method == "DELETE" && request.path == "/api/vehicles") {
    Session session;
    if (!authorize(request, client, session, "RENTER"))
      return;
    const int id = std::atoi(formValue(query, "id").c_str());
    Statement remove(
        Database::instance().handle(),
        "UPDATE vehicles SET is_active=0 WHERE id=? AND owner_id=? "
        "AND is_active=1");
    remove.bind(1, id);
    remove.bind(2, session.id);
    remove.step();
    if (sqlite3_changes(Database::instance().handle()) == 0) {
      fail(client, 400, "Vehicle not found or you do not own it.");
      return;
    }
    jsonResponse(client, 200,
                 "{\"ok\":true,\"message\":\"Vehicle removed from the active "
                 "fleet. Booking history was preserved.\"}");
    return;
  }

  if (request.method == "POST" && request.path == "/api/bookings") {
    Session session;
    if (!authorize(request, client, session, "CUSTOMER"))
      return;
    const int vehicleId = std::atoi(formValue(values, "vehicleId").c_str());
    try {
      const int id = BookingService().book(
          session.id, vehicleId, formValue(values, "startDate"),
          formValue(values, "endDate"), formValue(values, "pickupLocation"));
      jsonResponse(client, 201,
                   "{\"ok\":true,\"id\":" + std::to_string(id) + "}");
    } catch (const std::exception &error) {
      fail(client, 400, error.what());
    }
    return;
  }

  if (request.method == "GET" && request.path == "/api/bookings") {
    Session session;
    if (!authorize(request, client, session))
      return;
    const char *sql =
        session.role == "ADMIN"
            ? "SELECT "
              "b.id,b.customer_id,u.name,v.id,v.name,v.category,v.fuel,v.rate_"
              "per_day,b.start_date,b.end_date,b.total_amount,b.status FROM "
              "bookings b JOIN users u ON u.id=b.customer_id JOIN vehicles v "
              "ON v.id=b.vehicle_id ORDER BY b.id DESC"
        : session.role == "RENTER"
            ? "SELECT "
              "b.id,b.customer_id,u.name,v.id,v.name,v.category,v.fuel,v.rate_"
              "per_day,b.start_date,b.end_date,b.total_amount,b.status FROM "
              "bookings b JOIN users u ON u.id=b.customer_id JOIN vehicles v "
              "ON v.id=b.vehicle_id WHERE v.owner_id=? ORDER BY b.id DESC"
            : "SELECT "
              "b.id,b.customer_id,u.name,v.id,v.name,v.category,v.fuel,v.rate_"
              "per_day,b.start_date,b.end_date,b.total_amount,b.status FROM "
              "bookings b JOIN users u ON u.id=b.customer_id JOIN vehicles v "
              "ON v.id=b.vehicle_id WHERE b.customer_id=? ORDER BY b.id DESC";
    Statement rows(Database::instance().handle(), sql);
    if (session.role != "ADMIN")
      rows.bind(1, session.id);
    std::string out = "{\"bookings\":[";
    bool first = true;
    while (rows.step()) {
      if (!first)
        out += ',';
      first = false;
      out += "{\"id\":" + std::to_string(rows.integer(0)) +
             ",\"customerId\":" + std::to_string(rows.integer(1)) +
             ",\"customerName\":" + quote(rows.text(2)) +
             ",\"vehicleId\":" + std::to_string(rows.integer(3)) +
             ",\"vehicleName\":" + quote(rows.text(4)) +
             ",\"vehicleType\":" + quote(rows.text(5)) +
             ",\"fuel\":" + quote(rows.text(6)) +
             ",\"ratePerDay\":" + std::to_string(rows.real(7)) +
             ",\"startDate\":" + quote(rows.text(8)) +
             ",\"endDate\":" + quote(rows.text(9)) +
             ",\"total\":" + std::to_string(rows.real(10)) +
             ",\"status\":" + quote(rows.text(11)) + "}";
    }
    jsonResponse(client, 200, out + "]}");
    return;
  }

  if (request.method == "POST" && request.path == "/api/payments") {
    Session session;
    if (!authorize(request, client, session, "CUSTOMER"))
      return;
    const int bookingId = std::atoi(formValue(values, "bookingId").c_str());
    Statement booking(
        Database::instance().handle(),
        "SELECT total_amount FROM bookings WHERE id=? AND customer_id=? "
        "AND status IN ('ACTIVE','RETURNED')");
    booking.bind(1, bookingId);
    booking.bind(2, session.id);
    if (!booking.step()) {
      fail(client, 400, "Unpaid booking for this customer was not found.");
      return;
    }
    const double amount = booking.real(0);
    Statement existing(
        Database::instance().handle(),
        "SELECT status FROM payments WHERE booking_id=? "
        "AND status IN ('PENDING','PAID') ORDER BY id DESC LIMIT 1");
    existing.bind(1, bookingId);
    if (existing.step()) {
      fail(client, 400,
           existing.text(0) == "PAID"
               ? "This booking has already been paid."
               : "Payment for this booking is already processing.");
      return;
    }
    Statement insert(Database::instance().handle(),
                     "INSERT INTO payments(booking_id,amount,method,status) "
                     "VALUES(?,?,'DEMO','PENDING')");
    insert.bind(1, bookingId);
    insert.bind(2, amount);
    insert.step();
    const int paymentId = static_cast<int>(
        sqlite3_last_insert_rowid(Database::instance().handle()));
    try {
      paymentWorker.enqueue(paymentId);
    } catch (const std::exception &error) {
      Statement failed(Database::instance().handle(),
                       "UPDATE payments SET status='FAILED' WHERE id=?");
      failed.bind(1, paymentId);
      failed.step();
      fail(client, 400, error.what());
      return;
    }
    jsonResponse(client, 201,
                 "{\"ok\":true,\"paymentId\":" + std::to_string(paymentId) +
                     ",\"status\":\"PENDING\",\"message\":\"Demo payment is "
                     "processing.\"}");
    return;
  }

  if (request.method == "GET" && request.path == "/api/payments") {
    Session session;
    if (!authorize(request, client, session))
      return;
    const char *sql =
        session.role == "ADMIN"
            ? "SELECT "
              "p.id,p.booking_id,p.amount,p.method,p.status,p.transaction_"
              "ref FROM payments p ORDER BY p.id DESC"
        : session.role == "RENTER"
            ? "SELECT "
              "p.id,p.booking_id,p.amount,p.method,p.status,p.transaction_"
              "ref FROM payments p JOIN bookings b ON b.id=p.booking_id JOIN "
              "vehicles v ON v.id=b.vehicle_id WHERE v.owner_id=? ORDER BY "
              "p.id "
              "DESC"
            : "SELECT "
              "p.id,p.booking_id,p.amount,p.method,p.status,p.transaction_"
              "ref FROM payments p JOIN bookings b ON b.id=p.booking_id WHERE "
              "b.customer_id=? ORDER BY p.id DESC";
    Statement rows(Database::instance().handle(), sql);
    if (session.role != "ADMIN")
      rows.bind(1, session.id);
    std::string out = "{\"payments\":[";
    bool first = true;
    while (rows.step()) {
      if (!first)
        out += ',';
      first = false;
      out += "{\"id\":" + std::to_string(rows.integer(0)) +
             ",\"bookingId\":" + std::to_string(rows.integer(1)) +
             ",\"amount\":" + std::to_string(rows.real(2)) +
             ",\"method\":" + quote(rows.text(3)) +
             ",\"status\":" + quote(rows.text(4)) +
             ",\"transactionRef\":" + quote(rows.text(5)) + "}";
    }
    jsonResponse(client, 200, out + "]}");
    return;
  }

  if (request.method == "POST" && request.path == "/api/return") {
    Session session;
    if (!authorize(request, client, session, "CUSTOMER"))
      return;
    const int bookingId = std::atoi(formValue(values, "bookingId").c_str());
    try {
      BookingService().returnVehicle(bookingId, session.id);
      jsonResponse(client, 200, "{\"ok\":true}");
    } catch (const std::exception &error) {
      fail(client, 400, error.what());
    }
    return;
  }

  if (request.method == "GET" && request.path == "/api/users") {
    Session session;
    if (!authorize(request, client, session, "ADMIN"))
      return;
    const std::string role =
        lower(formValue(query, "role")) == "renter" ? "RENTER" : "CUSTOMER";
    Statement rows(Database::instance().handle(),
                   "SELECT id,name,email,phone,role,license_no FROM users "
                   "WHERE role=? ORDER BY id");
    rows.bind(1, role);
    std::string out = "{\"users\":[";
    bool first = true;
    while (rows.step()) {
      if (!first)
        out += ',';
      first = false;
      out += userJson(rows.integer(0), rows.text(1), rows.text(2), rows.text(3),
                      rows.text(4), rows.text(5));
    }
    jsonResponse(client, 200, out + "]}");
    return;
  }

  fail(client, 404, "API endpoint not found.");
}
} // namespace

int runHttpServer(int argc, char **argv) {
  try {
    Database::instance();
    if (argc > 1 && std::string(argv[1]) == "--create-admin") {
      createAdmin();
      return 0;
    }
    serverRunning = 1;
    std::signal(SIGINT, handleInterrupt);
    PaymentWorker paymentWorker;
    WSADATA data;
    if (WSAStartup(MAKEWORD(2, 2), &data) != 0)
      throw RentalError("Could not start Windows socket support.");
    SOCKET listener = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (listener == INVALID_SOCKET) {
      WSACleanup();
      throw RentalError("Could not create local HTTP socket.");
    }
    sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_port = htons(8080);
    address.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    if (bind(listener, reinterpret_cast<sockaddr *>(&address),
             sizeof(address)) == SOCKET_ERROR ||
        listen(listener, 10) == SOCKET_ERROR) {
      closesocket(listener);
      WSACleanup();
      throw RentalError("Could not listen on http://127.0.0.1:8080 (is the "
                        "port already in use?).");
    }
    paymentWorker.start();
    std::cout << "RentRide backend running at http://127.0.0.1:8080\nPress "
                 "Ctrl+C to stop.\n";
    while (serverRunning) {
      fd_set readable;
      FD_ZERO(&readable);
      FD_SET(listener, &readable);
      timeval wait = {0, 250000};
      const int ready = select(0, &readable, NULL, NULL, &wait);
      if (ready == SOCKET_ERROR) {
        if (!serverRunning)
          break;
        continue;
      }
      if (ready == 0)
        continue;
      SOCKET client = accept(listener, NULL, NULL);
      if (client == INVALID_SOCKET) {
        if (!serverRunning)
          break;
        continue;
      }
      HttpRequest request;
      if (readRequest(client, request)) {
        try {
          handle(client, request, paymentWorker);
        } catch (const std::exception &error) {
          fail(client, 400, error.what());
        }
      }
      shutdown(client, SD_SEND);
      closesocket(client);
    }
    closesocket(listener);
    paymentWorker.shutdown();
    std::signal(SIGINT, SIG_DFL);
    WSACleanup();
  } catch (const std::exception &error) {
    std::cerr << "Backend error: " << error.what() << '\n';
    return 1;
  }
  return 0;
}
