#include <iostream>
#include <vector>
#include <string>
#include <exception>
using namespace std;

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

    // Basic-to-class typecasting constructor
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

    // CLASS --> BASIC
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
    class Discount // nested
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

    // Exception Handling - 1
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

    // Function overloading: normal bill
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

    // Function overloading: bill with discount
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
        // operator overload
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
    // virtual void display() =0;
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
    // Normal bill
    void showBill(int id)
    {
        double bill = db.showBill_D(id);

        if (bill != -1)
            cout << "Total Bill: Rs. " << bill << endl;
    }

    // Bill with discount
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

    // vector<renter> renterList;
    // vector<customer> customerList;

    // // Pointer to object
    // Vehicle v1("Toyota", 1500, 101, true);
    // Vehicle *ptr = &v1;
    // ptr->displayVehicle();

    // // operator overload
    // renter r(2021, "parth", 960, db);
    // cout << r;

    // Class -> Basic
    Vehicle v1("Toyota", 1500, 101, true);
    double price = v1;
    cout << price;
    cout << endl;

    double value = 2000;
    Vehicle v2 = value;
    v2.displayVehicle();
    cout << endl;

    // Template function
    database::Discount d;
    double b1 = d.applyDiscount(7500, 55);
    double b2 = d.applyDiscount(75.5, 4.23);
    cout << b1 << endl
         << b2;

    // class to class typecasting
    UserInfo u1(101, "Parth", 960, db, "Renter");
    UserInfo u2(102, "Rahul", 987654, db, "Customer");

    renter r2 = u1;
    customer c2 = u2;

    cout << r2;
    cout << c2;

    return 0;
}