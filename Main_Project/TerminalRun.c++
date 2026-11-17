#include <iostream>
#include <vector>
#include <string>
#include <exception>
#include <limits>
using namespace std;

// Reads a validated int. On bad input (letters) or EOF (Ctrl+D / piped
// input running out), this clears the failed stream and either asks
// again or exits cleanly instead of spinning forever.
int readInt(const string &prompt)
{
    int value;
    while (true)
    {
        cout << prompt;
        if (cin >> value)
        {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return value;
        }
        if (cin.eof())
        {
            cout << "\nInput closed. Exiting.\n";
            exit(0);
        }
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid number, please try again.\n";
    }
}

double readDouble(const string &prompt)
{
    double value;
    while (true)
    {
        cout << prompt;
        if (cin >> value)
        {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return value;
        }
        if (cin.eof())
        {
            cout << "\nInput closed. Exiting.\n";
            exit(0);
        }
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid number, please try again.\n";
    }
}

class Vehicle
{
private:
    string vehicle_name;
    double vehicle_price;
    int vehicle_id;
    bool vehicle_available;
    int vehicle_rentedDay = 0;

public:
    Vehicle(string name, double price, int id, bool available)
    {
        vehicle_name = name;
        vehicle_price = price;
        vehicle_id = id;
        vehicle_available = available;
    }

    Vehicle(double price)
    {
        vehicle_name = "Unknown";
        vehicle_price = price;
        vehicle_id = 0;
        vehicle_available = true;
    }

    void markRented()
    {
        vehicle_available = false;
    }
    void markAvailable()
    {
        vehicle_available = true;
    }
    void displayVehicle()
    {
        cout << "----------------------------" << endl;
        cout << "Vehicle Name: " << vehicle_name << endl;
        cout << "Vehicle ID: " << vehicle_id << endl;
        cout << "Vehicle Price Per Day: " << vehicle_price << endl;
        cout << "Vehicle Available: " << vehicle_available << endl;
        cout << "----------------------------" << endl;
    }

    friend ostream &operator<<(ostream &out, const Vehicle &v)
    {
        out << "----------------------------" << endl;
        out << "Vehicle Name: " << v.vehicle_name << endl;
        out << "Vehicle ID: " << v.vehicle_id << endl;
        out << "Vehicle Price Per Day: " << v.vehicle_price << endl;
        out << "Vehicle Available: " << v.vehicle_available << endl;
        out << "----------------------------" << endl;
        return out;
    }

    bool isAvailable()
    {
        return vehicle_available;
    }
    double getPricePerDay()
    {
        return vehicle_price;
    }
    int getVehicleId()
    {
        return vehicle_id;
    }
    string getVehicleName()
    {
        return vehicle_name;
    }
    void setRentedDay(int day)
    {
        vehicle_rentedDay = day;
    }
    int getRentedDay()
    {
        return vehicle_rentedDay;
    }

    operator double()
    {
        return vehicle_price;
    }
};

class database
{
private:
    vector<Vehicle> vehicleList;
    static int vehicleCount;
    friend void showVehicleListToAdmin(database &db);

public:
    class Discount
    {
    public:
        template <typename T>
        T applyDiscount(T bill, T percentage)
        {
            return bill - (bill * percentage / 100.0);
        }
    };
    void addVehicle(string name, double price, int id, bool available)
    {
        vehicleCount++;
        Vehicle v(name, price, id, available);
        vehicleList.push_back(v);
    }

    void removeVehicle(int id)
    {
        int i = 0;
        while (i < vehicleCount && vehicleList[i].getVehicleId() != id)
        {
            i++;
        }
        if (i < vehicleCount && vehicleList[i].getVehicleId() == id)
        {
            while (i < vehicleCount - 1)
            {
                vehicleList[i] = vehicleList[i + 1];
                i++;
            }
            vehicleList.pop_back();
            vehicleCount--;
        }
        else
        {
            cout << "Vehicle Not Found!!!";
        }
    }

    void showVehicle_D()
    {
        for (auto &v : vehicleList)
        {
            cout << v;
        }
    }

    void showAvailableVehicle_D()
    {
        for (auto &v : vehicleList)
        {
            if (v.isAvailable())
            {
                cout << v;
            }
        }
    }

    void bookVehicle_D(int id, int day)
    {
        bool done = false;
        for (auto &v : vehicleList)
        {
            if (v.getVehicleId() == id)
            {
                v.markRented();
                v.setRentedDay(day);
                cout << "Vehicle Booked";
                done = true;
                break;
            }
        }
        if (!done)
        {
            cout << "Vehicle Not Found!!!";
        }
    }

    void bookVehicle_D_ExceptionHandling(int id, int day)
    {
        try
        {
            bool done = false;
            for (auto &v : vehicleList)
            {
                if (v.getVehicleId() == id)
                {
                    v.markRented();
                    v.setRentedDay(day);
                    cout << "Vehicle Booked";
                    done = true;
                    break;
                }
            }

            if (!done)
            {
                throw 404;
            }
        }
        catch (int a)
        {
            cout<<"Error "<<a;
            cout << "Vehicle Not Found!!!";
        }
    }

    void showBooking_D()
    {
        for (auto &v : vehicleList)
        {
            if (!v.isAvailable())
            {
                cout << v;
            }
        }
    }

    void returnVehicle_D(int id)
    {
        bool done = false;
        for (auto &v : vehicleList)
        {
            if (v.getVehicleId() == id)
            {
                v.markAvailable();
                v.setRentedDay(0);
                cout << "Vehicle Returned!!!";
                done = true;
                break;
            }
        }
        if (!done)
        {
            cout << "Vehicle Not Found!!!";
        }
    }

    double showBill_D(int id)
    {
        for (auto &v : vehicleList)
        {
            if (v.getVehicleId() == id)
            {
                return v.getPricePerDay() * v.getRentedDay();
            }
        }

        cout << "Vehicle Not Found!!!\n";
        return -1;
    }

    double showBill_D(int id, double discountPercent)
    {
        double bill = showBill_D(id);

        if (bill == -1)
            return -1;

        if (discountPercent < 0 || discountPercent > 100)
        {
            cout << "Invalid discount percentage!\n";
            return -1;
        }

        Discount d;
        return d.applyDiscount(bill, discountPercent);
    }
};
int database::vehicleCount = 0;

void showVehicleListToAdmin(database &db)
{
    cout << "\n===== ADMIN: VEHICLE LIST =====\n";

    for (auto &v : db.vehicleList)
    {
        cout << v;
    }
}

class UserInfo
{
public:
    string name;
    int id;
    int phone;
    string role;
    database *db;
    string license;

    UserInfo(int i, string n, int p, database &d,
             string userRole = "", string lic = "")
        : name(n), id(i), phone(p), role(userRole), db(&d), license(lic) {}

    void display()
    {
        cout << "Role: " << role << endl;
        cout << "Name: " << name << endl;
        cout << "Phone: " << phone << endl;
        cout << "ID: " << id << endl;
    }
};

class user
{
protected:
    string name;
    int id, phone;

public:
    user(int id, string name, int phone)
        : id(id), name(name), phone(phone) {}

    virtual void displayUser() = 0;

    virtual ~user() = default;
};

class renter : public user
{
private:
    database &db;

public:
    renter(int id, string name, int phone, database &db)
        : user(id, name, phone), db(db)
    {
    }

    renter(const UserInfo &u)
        : user(u.id, u.name, u.phone), db(*u.db)
    {
    }

    friend ostream &operator<<(ostream &out, const renter &r)
    {
        out << "Role: Renter\n"
            << "ID: " << r.id << "\n"
            << "Name: " << r.name << "\n"
            << "Phone: " << r.phone << "\n";

        return out;
    }

    void displayUser() override
    {
        cout << "Role: Renter\n"
             << "ID: " << id << "\n"
             << "Name: " << name << "\n"
             << "Phone: " << phone << "\n";
    }

    void showVehicles_R()
    {
        db.showVehicle_D();
    }

    void showAvailable_R()
    {
        db.showAvailableVehicle_D();
    }

    void addVehicle(string name, double price, int id, bool available)
    {
        db.addVehicle(name, price, id, available);
    }

    void removeVehicle(int vehicle_id)
    {
        db.removeVehicle(vehicle_id);
    }
    ~renter() {};
};

class customer : public user
{
protected:
    string drivingLicenseNo;
    database &db;

public:
    friend ostream &operator<<(ostream &out, const customer &c)
    {
        out << "Role: Customer\n"
            << "ID: " << c.id << "\n"
            << "Name: " << c.name << "\n"
            << "Phone: " << c.phone << "\n";

        return out;
    }

    customer(string name, int id, int phone, string license, database &db) : user(id, name, phone), db(db)
    {
        drivingLicenseNo = license;
    }

    customer(const UserInfo &u) : user(u.id, u.name, u.phone),
                                  drivingLicenseNo(u.license), db(*u.db)
    {
    }
    void displayUser() override
    {
        cout << "Role :" << "Customer\n";
        cout << "Id: " << id;
        cout << "Name of Customer: " << name;
        cout << "Driving License Number: " << drivingLicenseNo;
        cout << "Phone: " << phone;
    }

    void showVehicles_C()
    {
        db.showVehicle_D();
    }
    void returnVehicle_C(int id)
    {
        db.returnVehicle_D(id);
    }
    void showAvailableVehicles_C()
    {
        db.showAvailableVehicle_D();
    }
    void bookVehicles_C(int id, int day)
    {
        db.bookVehicle_D(id, day);
    }
    void showBookings_C()
    {
        db.showBooking_D();
    }
    void showBill(int id)
    {
        double bill = db.showBill_D(id);

        if (bill != -1)
            cout << "Total Bill: Rs. " << bill << endl;
    }

    void showBill(int id, double discountPercent)
    {
        double bill = db.showBill_D(id, discountPercent);

        if (bill != -1)
            cout << "Final Bill after Discount: Rs. " << bill << endl;
    }

    ~customer() {};
};
class admin final : public user
{
public:
    admin(int id, string name, int phone)
        : user(id, name, phone)
    {
    }

    void displayUser() override final
    {
        cout << "Role: Admin\n"
             << "ID: " << id << "\n"
             << "Name: " << name << "\n"
             << "Phone: " << phone << "\n";
    }
};

int main()
{
    database db;

    // Dummy/sample vehicles so the system has data to work with on startup
    db.addVehicle("Honda City", 2500.0, 101, true);
    db.addVehicle("Hyundai Creta", 3200.0, 102, true);
    db.addVehicle("Maruti Swift", 1800.0, 103, true);
    db.addVehicle("Toyota Innova", 4500.0, 104, true);
    db.addVehicle("Royal Enfield Classic 350", 1200.0, 105, true);
    db.addVehicle("Activa 6G", 600.0, 106, true);

    vector<renter> renterList;
    vector<customer> customerList;

    int choice;

    do
    {
        cout << "\n\n===== VEHICLE RENTAL MANAGEMENT SYSTEM =====\n";
        cout << "1. Renter Login / Register\n";
        cout << "2. Customer Login / Register\n";
        cout << "3. Admin Panel\n";
        cout << "0. Exit\n";
        choice = readInt("Enter choice: ");

        if (choice == 1)
        {
            int id, phone;
            string name;

            cout << "\n--- Renter Registration ---\n";
            id = readInt("Enter renter ID: ");

            cout << "Enter renter name: ";
            cin >> name;

            phone = readInt("Enter phone number: ");

            renterList.emplace_back(id, name, phone, db);
            renter &r = renterList.back();

            int ch;

            do
            {
                cout << "\n===== RENTER MENU =====\n";
                cout << "1. Add Vehicle\n";
                cout << "2. Remove Vehicle\n";
                cout << "3. Show All Vehicles\n";
                cout << "4. Show Available Vehicles\n";
                cout << "0. Back to Main Menu\n";
                ch = readInt("Enter choice: ");

                if (ch == 1)
                {
                    string name;
                    double price;
                    int vid;
                    bool available = true;

                    cout << "Enter vehicle name: ";
                    cin >> name;

                    price = readDouble("Enter price per day: ");

                    vid = readInt("Enter vehicle ID: ");

                    r.addVehicle(name, price, vid, available);

                    cout << "Vehicle Added Successfully!\n";
                }
                else if (ch == 2)
                {
                    int vid;

                    vid = readInt("Enter vehicle ID to remove: ");

                    r.removeVehicle(vid);
                }
                else if (ch == 3)
                {
                    r.showVehicles_R();
                }
                else if (ch == 4)
                {
                    r.showAvailable_R();
                }

            } while (ch != 0);
        }

        else if (choice == 2)
        {
            int id, phone;
            string name, license;

            cout << "\n--- Customer Registration ---\n";
            id = readInt("Enter customer ID: ");

            cout << "Enter customer name: ";
            cin >> name;

            phone = readInt("Enter phone number: ");

            cout << "Enter driving license number: ";
            cin >> license;

            customerList.emplace_back(name, id, phone, license, db);
            customer &c = customerList.back();

            int ch;

            do
            {
                cout << "\n===== CUSTOMER MENU =====\n";
                cout << "1. Show All Vehicles\n";
                cout << "2. Show Available Vehicles\n";
                cout << "3. Book Vehicle\n";
                cout << "4. Show Bookings\n";
                cout << "5. Calculate Bill\n";
                cout << "6. Return Vehicle\n";
                cout << "7. Calculate Bill with Discount\n";
                cout << "0. Back to Main Menu\n";
                ch = readInt("Enter choice: ");

                if (ch == 1)
                {
                    c.showVehicles_C();
                }
                else if (ch == 2)
                {
                    c.showAvailableVehicles_C();
                }
                else if (ch == 3)
                {
                    int vid, days;

                    vid = readInt("Enter vehicle ID: ");

                    days = readInt("Enter rental days: ");

                    if (days > 0)
                    {
                        c.bookVehicles_C(vid, days);
                    }
                    else
                    {
                        cout << "Days must be greater than zero!\n";
                    }
                }
                else if (ch == 4)
                {
                    c.showBookings_C();
                }
                else if (ch == 5)
                {
                    int vid;

                    vid = readInt("Enter vehicle ID: ");

                    c.showBill(vid);
                }
                else if (ch == 6)
                {
                    int vid;

                    vid = readInt("Enter vehicle ID to return: ");

                    c.returnVehicle_C(vid);
                }
                else if (ch == 7)
                {
                    int vid;
                    double discount;

                    vid = readInt("Enter vehicle ID: ");

                    discount = readDouble("Enter discount percentage: ");

                    c.showBill(vid, discount);
                }

            } while (ch != 0);
        }

        else if (choice == 3)
        {
            int ch;

            do
            {
                cout << "\n===== ADMIN PANEL =====\n";
                cout << "1. Show All Vehicles\n";
                cout << "2. Show Available Vehicles\n";
                cout << "3. Show Booked Vehicles\n";
                cout << "4. Show All Renters\n";
                cout << "5. Show All Customers\n";
                cout << "0. Back to Main Menu\n";
                cout << "Enter choice: ";
                cin >> ch;

                if (ch == 1)
                {
                    showVehicleListToAdmin(db);
                }
                else if (ch == 2)
                {
                    db.showAvailableVehicle_D();
                }
                else if (ch == 3)
                {
                    db.showBooking_D();
                }
                else if (ch == 4)
                {
                    if (renterList.empty())
                    {
                        cout << "No renters registered.\n";
                    }

                    for (auto &r : renterList)
                    {
                        r.displayUser();
                        cout << "\n--------------------\n";
                    }
                }
                else if (ch == 5)
                {
                    if (customerList.empty())
                    {
                        cout << "No customers registered.\n";
                    }

                    for (auto &c : customerList)
                    {
                        c.displayUser();
                        cout << "\n--------------------\n";
                    }
                }

            } while (ch != 0);
        }

        else if (choice != 0)
        {
            cout << "Invalid choice! Try again.\n";
        }

    } while (choice != 0);

    cout << "\nThank you for using the system!\n";

    return 0;
}