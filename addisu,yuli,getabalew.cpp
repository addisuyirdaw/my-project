
#include <iostream>
#include <list>
#include <queue>
#include <iomanip>
#include "sqlite3.h"
using namespace std;

// Hotel structure
struct Hotel {
    int id;
    string name;
    string services;
    string location;
    int roomNumber;
};

// Guest structure
struct Guest {
    int id;
    string name;
    int queuePosition;
};

// Global Data
list<Hotel> hotelList;
queue<Guest> guestQueue;
list<string> itinerary;
sqlite3* db;

// Function prototypes
void initializeDatabase();
void closeDatabase();
void adminMenu();
void customerMenu();
bool authenticateAdmin();
void addHotel();
void updateHotel();
void viewHotels();
void deleteHotel();
void saveHotelToDatabase(const Hotel& hotel);
void loadHotelsFromDatabase();
void addGuest();
void serveGuest();
void displayGuestQueue();
void saveGuestToDatabase(const Guest& guest);
void loadGuestsFromDatabase();
void addStopToItinerary();
void viewItinerary();
bool isHotelIdUnique(int id);
bool isGuestIdUnique(int id);

// Predefined hotels near Lalibela
void addPredefinedHotels() {
    hotelList = {
        {1, "Lalibela Lodge", "Free Wi-Fi, Restaurant, Pool", "Lalibela", 20},
        {2, "Mountain View Hotel", "Spa, Free Breakfast", "Lalibela", 15},
        {3, "Rock-Hewn Inn", "Bar, Room Service", "Lalibela", 25},
        {4, "Zion Hotel", "Free Parking, Conference Room", "Lalibela", 30},
        {5, "St. George Guesthouse", "Free Wi-Fi, Garden", "Lalibela", 10},
        {6, "Axum Hotel", "Restaurant, Free Parking", "Lalibela", 18},
        {7, "Queen Sheba Hotel", "Pool, Spa, Gym", "Lalibela", 22},
        {8, "Ethiopian Heritage Hotel", "Free Breakfast, Bar", "Lalibela", 12},
        {9, "Bete Maryam Hotel", "Free Wi-Fi, Restaurant", "Lalibela", 14},
        {10, "Tekle Haymanot Lodge", "Garden, Room Service", "Lalibela", 16}
    };
}

// Predefined itineraries
void addPredefinedItinerary() {
    itinerary = {
        "1. Visit the Rock-Hewn Churches of Lalibela",
        "2. Explore Asheton Maryam Monastery",
        "3. Hike to the top of Mount Abuna Yosef",
        "4. Visit the Lalibela Market",
        "5. Attend a traditional coffee ceremony"
    };
}

int main() {
    initializeDatabase();
    loadHotelsFromDatabase();
    loadGuestsFromDatabase();

    int userType;
    do {
        cout << "\nLalibela Tourism Management\n";
        cout << "1. Admin\n2. Customer\n3. Exit\nChoice: ";
        cin >> userType;
        cin.ignore();

        switch(userType) {
            case 1:
                if(authenticateAdmin()) adminMenu();
                else cout << "Access denied!\n";
                break;
            case 2: customerMenu(); break;
            case 3: cout << "Exiting...\n"; break;
            default: cout << "Invalid choice!\n";
        }
    } while(userType != 3);

    closeDatabase();
    return 0;
}

// Initialize SQLite Database
void initializeDatabase() {
    if (sqlite3_open("tourism.db", &db)) {
        cerr << "Error opening SQLite database: " << sqlite3_errmsg(db) << endl;
        exit(1);
    }

    const char* createHotelsTable =
        "CREATE TABLE IF NOT EXISTS Hotels ("
        "id INTEGER PRIMARY KEY, "
        "name TEXT, "
        "services TEXT, "
        "location TEXT, "
        "roomNumber INTEGER);";

    const char* createGuestsTable =
        "CREATE TABLE IF NOT EXISTS Guests ("
        "id INTEGER PRIMARY KEY, "
        "name TEXT, "
        "queuePosition INTEGER);";

    char* errMsg;
    if (sqlite3_exec(db, createHotelsTable, nullptr, nullptr, &errMsg) != SQLITE_OK) {
        cerr << "Error creating Hotels table: " << errMsg << endl;
        sqlite3_free(errMsg);
        exit(1);
    }

    if (sqlite3_exec(db, createGuestsTable, nullptr, nullptr, &errMsg) != SQLITE_OK) {
        cerr << "Error creating Guests table: " << errMsg << endl;
        sqlite3_free(errMsg);
        exit(1);
    }

    cout << "Database initialized successfully.\n";
}

// Close SQLite Database
void closeDatabase() {
    sqlite3_close(db);
    cout << "Database connection closed.\n";
}



// Check if hotel ID is unique
bool isHotelIdUnique(int id) {
    for (const auto& hotel : hotelList) {
        if (hotel.id == id) {
            return false; // ID already exists
        }
    }
    return true; // ID is unique
}

// Check if guest ID is unique
bool isGuestIdUnique(int id) {
    queue<Guest> tempQueue = guestQueue;
    while (!tempQueue.empty()) {
        if (tempQueue.front().id == id) {
            return false; // ID already exists
        }
        tempQueue.pop();
    }
    return true; // ID is unique
}

// Admin authentication
bool authenticateAdmin() {
    string password;
    cout << "Enter admin password: ";
    getline(cin, password);
    return (password == "admin123"); // Simple password check
}

// Admin menu
void adminMenu() {
    int choice;
    do {
        cout << "\nAdmin Menu\n"
             << "1. Add Hotel\n"
             << "2. Update Hotel\n"
             << "3. View Hotels\n"
             << "4. Delete Hotel\n"
             << "5. Back\nChoice: ";
        cin >> choice;
        cin.ignore();

        switch(choice) {
            case 1: addHotel(); break;
            case 2: updateHotel(); break;
            case 3: viewHotels(); break;
            case 4: deleteHotel(); break;
        }
    } while(choice != 5);
}

// Add a new hotel
void addHotel() {
    Hotel hotel;
    cout << "Enter Hotel ID: ";
    cin >> hotel.id;
    cin.ignore();

    if (!isHotelIdUnique(hotel.id)) {
        cout << "Error: Hotel ID already exists. Please enter a unique ID.\n";
        return;
    }

    cout << "Enter Hotel Name: ";
    getline(cin, hotel.name);
    cout << "Enter Services: ";
    getline(cin, hotel.services);
    cout << "Enter Location: ";
    getline(cin, hotel.location);
    cout << "Enter Room Number: ";
    cin >> hotel.roomNumber;
    cin.ignore();

    hotelList.push_back(hotel);
    saveHotelToDatabase(hotel);
    cout << "Hotel added successfully!\n";
}

// Save hotel to database
void saveHotelToDatabase(const Hotel& hotel) {
    const char* sql = "INSERT INTO Hotels (id, name, services, location, roomNumber) VALUES (?, ?, ?, ?, ?);";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, hotel.id);
        sqlite3_bind_text(stmt, 2, hotel.name.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 3, hotel.services.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 4, hotel.location.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_int(stmt, 5, hotel.roomNumber);

        if (sqlite3_step(stmt) != SQLITE_DONE) {
            cerr << "Error inserting hotel into database: " << sqlite3_errmsg(db) << endl;
        }
        sqlite3_finalize(stmt);
    } else {
        cerr << "Error preparing insert statement: " << sqlite3_errmsg(db) << endl;
    }
}

// Load hotels from database
void loadHotelsFromDatabase() {
    const char* sql = "SELECT * FROM Hotels;";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            Hotel hotel;
            hotel.id = sqlite3_column_int(stmt, 0);
            hotel.name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
            hotel.services = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
            hotel.location = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
            hotel.roomNumber = sqlite3_column_int(stmt, 4);
            hotelList.push_back(hotel);
        }
        sqlite3_finalize(stmt);
    } else {
        cerr << "Error loading hotels from database: " << sqlite3_errmsg(db) << endl;
    }
}

// Update hotel details
void updateHotel() {
    int id;
    cout << "Enter Hotel ID to update: ";
    cin >> id;
    cin.ignore();




for (auto& hotel : hotelList) {
        if (hotel.id == id) {
            cout << "New name (" << hotel.name << "): ";
            getline(cin, hotel.name);
            cout << "New services (" << hotel.services << "): ";
            getline(cin, hotel.services);
            cout << "New location (" << hotel.location << "): ";
            getline(cin, hotel.location);
            cout << "New room number (" << hotel.roomNumber << "): ";
            cin >> hotel.roomNumber;
            cin.ignore();

            const char* sql = "UPDATE Hotels SET name=?, services=?, location=?, roomNumber=? WHERE id=?;";
            sqlite3_stmt* stmt;

            if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
                sqlite3_bind_text(stmt, 1, hotel.name.c_str(), -1, SQLITE_STATIC);
                sqlite3_bind_text(stmt, 2, hotel.services.c_str(), -1, SQLITE_STATIC);
                sqlite3_bind_text(stmt, 3, hotel.location.c_str(), -1, SQLITE_STATIC);
                sqlite3_bind_int(stmt, 4, hotel.roomNumber);
                sqlite3_bind_int(stmt, 5, hotel.id);

                if (sqlite3_step(stmt) != SQLITE_DONE) {
                    cerr << "Error updating hotel: " << sqlite3_errmsg(db) << endl;
                }
                sqlite3_finalize(stmt);
            } else {
                cerr << "Error preparing update statement: " << sqlite3_errmsg(db) << endl;
            }

            cout << "Hotel updated successfully!\n";
            return;
        }
    }
    cout << "Hotel not found!\n";
}

// View all hotels
void viewHotels() {
    cout << "\n--- Hotel List ---\n";
    for (const auto& hotel : hotelList) {
        cout << "ID: " << hotel.id << "\nName: " << hotel.name
             << "\nServices: " << hotel.services << "\nLocation: " << hotel.location
             << "\nRoom Number: " << hotel.roomNumber << "\n\n";
    }
}

// Delete a hotel
void deleteHotel() {
    int id;
    cout << "Enter Hotel ID to delete: ";
    cin >> id;
    cin.ignore();

    const char* sql = "DELETE FROM Hotels WHERE id=?;";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, id);

        if (sqlite3_step(stmt) == SQLITE_DONE) {
            hotelList.remove_if([id](const Hotel& hotel) { return hotel.id == id; });
            cout << "Hotel deleted successfully!\n";
        } else {
            cerr << "Error deleting hotel: " << sqlite3_errmsg(db) << endl;
        }
        sqlite3_finalize(stmt);
    } else {
        cerr << "Error preparing delete statement: " << sqlite3_errmsg(db) << endl;
    }
}

// Customer menu
void customerMenu() {
    int choice;
    do {
        cout << "\nCustomer Menu\n"
             << "1. View Hotels\n"
             << "2. Join Queue\n"
             << "3. View Queue\n"
             << "4. Add Itinerary Stop\n"
             << "5. View Itinerary\n"
             << "6. Back\nChoice: ";
        cin >> choice;
        cin.ignore();

        switch(choice) {
            case 1: viewHotels(); break;
            case 2: addGuest(); break;
            case 3: displayGuestQueue(); break;
            case 4: addStopToItinerary(); break;
            case 5: viewItinerary(); break;
        }
    } while(choice != 6);
}

// Add a guest to the queue
void addGuest() {
    Guest guest;
    cout << "Enter Guest ID: ";
    cin >> guest.id;
    cin.ignore();

    if (!isGuestIdUnique(guest.id)) {
        cout << "Error: Guest ID already exists. Please enter a unique ID.\n";
        return;
    }

    cout << "Enter Guest Name: ";
    getline(cin, guest.name);

    guest.queuePosition = guestQueue.size() + 1;
    guestQueue.push(guest);
    saveGuestToDatabase(guest);
    cout << "Guest added to queue! Position: " << guest.queuePosition << "\n";
}

// Save guest to database
void saveGuestToDatabase(const Guest& guest) {
    const char* sql = "INSERT INTO Guests (id, name, queuePosition) VALUES (?, ?, ?);";
    sqlite3_stmt* stmt;





if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, guest.id);
        sqlite3_bind_text(stmt, 2, guest.name.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_int(stmt, 3, guest.queuePosition);

        if (sqlite3_step(stmt) != SQLITE_DONE) {
            cerr << "Error inserting guest into database: " << sqlite3_errmsg(db) << endl;
        }
        sqlite3_finalize(stmt);
    } else {
        cerr << "Error preparing insert statement: " << sqlite3_errmsg(db) << endl;
    }
}

// Load guests from database
void loadGuestsFromDatabase() {
    const char* sql = "SELECT * FROM Guests;";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            Guest guest;
            guest.id = sqlite3_column_int(stmt, 0);
            guest.name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
            guest.queuePosition = sqlite3_column_int(stmt, 2);
            guestQueue.push(guest);
        }
        sqlite3_finalize(stmt);
    } else {
        cerr << "Error loading guests from database: " << sqlite3_errmsg(db) << endl;
    }
}

// Serve the next guest in the queue
void serveGuest() {
    if (!guestQueue.empty()) {
        Guest guest = guestQueue.front();
        cout << "Serving guest: " << guest.name << " (ID: " << guest.id << ")\n";
        guestQueue.pop();

        const char* sql = "DELETE FROM Guests WHERE id=?;";
        sqlite3_stmt* stmt;

        if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
            sqlite3_bind_int(stmt, 1, guest.id);

            if (sqlite3_step(stmt) != SQLITE_DONE) {
                cerr << "Error deleting guest from database: " << sqlite3_errmsg(db) << endl;
            }
            sqlite3_finalize(stmt);
        } else {
            cerr << "Error preparing delete statement: " << sqlite3_errmsg(db) << endl;
        }
    } else {
        cout << "Queue is empty!\n";
    }
}

// Display the guest queue
void displayGuestQueue() {
    cout << "\n--- Guest Queue ---\n";
    queue<Guest> tempQueue = guestQueue;
    while (!tempQueue.empty()) {
        Guest guest = tempQueue.front();
        cout << "ID: " << guest.id << ", Name: " << guest.name
             << ", Position: " << guest.queuePosition << "\n";
        tempQueue.pop();
    }
}

// Add a stop to the itinerary
void addStopToItinerary() {
    string stop;
    cout << "Enter stop: ";
    getline(cin, stop);
    itinerary.push_back(stop);
    cout << "Stop added to itinerary!\n";
}

// View the itinerary
void viewItinerary() {
    cout << "\n--- Itinerary ---\n";
    for (const auto& stop : itinerary) {
        cout << stop << "\n";
    }
}
