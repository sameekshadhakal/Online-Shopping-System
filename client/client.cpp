#include <iostream>
#include<Winsock2.h>
#include<WS2tcpip.h>
#include<tchar.h>
#include<thread>
#include<string>
#include <map>
#include <sstream>
#include <fstream>
#include <vector>
#include <iomanip>

using namespace std;

#pragma comment(lib, "ws2_32.lib")

// Data structures
map<string, map<string, double>> items;  // Category -> Item -> Price
map<string, map<string, int>> stock;     // Category -> Item -> Stock
map<string, map<string, int>> selected_items;      // Customer's Cart (Category -> Item -> Quantity)
map<string, string> buyerCredentials = { {"default_user", "default_pass"} }; // Predefined buyer credentials
const string sellerUsername = "admin";
const string sellerPassword = "admin123";
string username = "Guest";

// File to store buyer credentials
const string credentialsFile = "credentials.txt";

// Function to load buyer credentials from file
void loadCredentials() {
    ifstream infile(credentialsFile);
    if (infile.is_open()) {
        string username, password;
        while (infile >> username >> password) {
            buyerCredentials[username] = password;
        }
        infile.close();
    }
    else {
        cout << "No previous credentials found. Starting fresh.\n";
    }
}

void clearScreen() {
    system("cls");
    cout << "SMARTSHOP" << endl;
}
// Function to save a new buyer credential to file
void saveCredential(const string& username, const string& password) {
    ofstream outfile(credentialsFile, ios::app); // Open in append mode
    if (outfile.is_open()) {
        outfile << username << " " << password << "\n";
        outfile.close();
    }
    else {
        cout << "Error: Unable to save credentials to file.\n";
    }
}

// Function to handle buyer sign-up
void buyerSignup() {
    string  password;
    cout << "Enter a new username: ";
    cin >> username;
    cout << "Enter a new password: ";
    cin >> password;

    if (buyerCredentials.find(username) != buyerCredentials.end()) {
        cout << "Username already exists. Please try a different one.\n";
    }
    else {
        buyerCredentials[username] = password;
        saveCredential(username, password); // Save to file
        cout << "Signup successful! You can now login.\n";
        
    }
}

// Function to handle buyer login
bool buyerLogin(string& username) {
    cout << "Enter username: ";
    cin >> username;
    cout << "Enter password: ";
    string password;
    cin >> password;

    if (buyerCredentials.find(username) != buyerCredentials.end() && buyerCredentials[username] == password) {
        cout << "Login successful!\n";
        clearScreen();
        return true;
    }
    else {
        cout << "Invalid credentials. Please try again or sign up.\n";
        return false;
    }
}

// Function to handle seller login
bool sellerLogin() {
    cout << "Enter seller username: ";
    string username;
    cin >> username;
    cout << "Enter seller password: ";
    string password;
    cin >> password;

    if (username == sellerUsername && password == sellerPassword) {
        cout << "Seller login successful!\n";
        clearScreen();
        return true;
    }
    else {
        cout << "Invalid seller credentials.\n";
        return false;
    }
}


// Function to load items and stock from a file
void loadItemsFromFile(const string& category) {
    ifstream file(category + ".txt");
    string item;
    double price;
    int quantity;
    while (file >> item >> price >> quantity) {
        items[category][item] = price;
        stock[category][item] = quantity;
    }
}

void updateStockToFile(const string& category) {
    ofstream file(category + ".txt");
    if (!file.is_open()) {
        cout << "Failed to open the file for writing!" << endl;
        return;
    }

    for (const auto& entry : stock[category]) {
        file << entry.first << " " << items[category][entry.first] << " " << entry.second << endl;
    }

    file.close();
}

void viewTransactionHistory() {
    ifstream infile("customerrecords.txt");

    if (!infile.is_open()) {
        cout << "Error: Unable to open the transaction history file.\n";
        return;
    }

    cout << "\n--- Transaction History ---\n";
    string line;
    bool hasContent = false;

    while (getline(infile, line)) {
        hasContent = true;
        cout << line << "\n";
    }

    infile.close();

    if (!hasContent) {
        cout << "No transactions found in the history.\n";
    }
    else {
        cout << "\n--- End of Transaction History ---\n";
    }

    // Offer the user a choice to return to the menu or exit
    int choice;
    while (true) {
        cout << "\nOptions:\n";
        cout << "1. Return to Menu\n";
        cout << "2. Exit\n";
        cout << "Enter your choice: ";
        cin >> choice;

        switch (choice) {
        case 1:
            cout << "Returning to menu...\n";
            clearScreen();
            return; // Exit the loop and return to the menu
        case 2:
            cout << "Exiting program. Goodbye!\n";
            clearScreen();
            exit(0); // Terminate the program
        default:
            cout << "Invalid choice. Please try again.\n";
        }
        return;
    }
    
}

// Class customer
class customer {
public:
    string name;
    map<string, map<string, pair<int, double>>> cart;  // category -> item -> {quantity, price}


    customer() {
        cout << "--WELCOME TO SMARTSHOP--\n\n";
    }

    void addToCart(string category, string item, int quantity) {
        selected_items[category][item] += quantity;  // Correct quantity update
    }

    void displayBill(ostream& out, bool showHeader = true) {
        const int widthCategory = 20; // Width for Category column
        const int widthItem = 20;     // Width for Item column
        const int widthQuantity = 10; // Width for Quantity column
        const int widthCost = 10;     // Width for Cost column

        int total_amount = 0;
        cout << endl<<"Customer name :" << username << endl;

        if (showHeader) {
            out << std::left // Align text to the left
                << std::setw(widthCategory) << "Category"
                << std::setw(widthItem) << "Item"
                << std::setw(widthQuantity) << "Quantity"
                << std::setw(widthCost) << "Cost"
                << "\n";
            out << std::string(widthCategory + widthItem + widthQuantity + widthCost, '-') << "\n"; // Separator line
        }

        for (auto& categoryPair : selected_items) {
            for (auto& itemPair : categoryPair.second) {
                string item = itemPair.first;
                int quantity = itemPair.second;

                if (items[categoryPair.first].find(item) != items[categoryPair.first].end()) {
                    double cost = quantity * items[categoryPair.first][item];
                    out << std::left
                        << std::setw(widthCategory) << categoryPair.first
                        << std::setw(widthItem) << item
                        << std::setw(widthQuantity) << quantity
                        << std::setw(widthCost) << cost
                        << "\n";
                    total_amount += cost;
                }
                else {
                    out << std::left
                        << std::setw(widthCategory) << categoryPair.first
                        << std::setw(widthItem) << item
                        << std::setw(widthQuantity) << quantity
                        << std::setw(widthCost) << "ERROR"
                        << "\n";
                }
            }
        }

        out << std::string(widthCategory + widthItem + widthQuantity + widthCost, '-') << "\n";
        out << std::left
            << std::setw(widthCategory + widthItem + widthQuantity) << "Total Amount:"
            << std::setw(widthCost) << total_amount
            << "\n";
        out << std::string(widthCategory + widthItem + widthQuantity + widthCost, '-') << "\n";
    }
    void viewCart() {
        cout << "\n--- Your Cart ---\n";
        displayBill(cout);
    }

    void printBill() {
        string name;
        
        cout << "\n--- Final Bill ---\n";
      
        displayBill(cout);

        // Write the bill to the file
        ofstream outfile("customerrecords.txt", ios::app);
        if (outfile.is_open()) {
            displayBill(outfile, false); // Don't repeat the header for the file
            outfile << "*****THANK YOU && HAPPY ONLINE SHOPPING*****\n\n";
            outfile.close();
        }
        else {
            cout << "Error: Unable to open the file for writing.\n";
        }

        cout << "*****THANK YOU && HAPPY ONLINE SHOPPING*****\n";
    }
 

};



// Class shop and its subclasses
class shop {
public:

    virtual void show() {}
    virtual void select(customer& obj) {}

    void showMenu() {
        cout << "                 Menu\n";
        cout << " -----------------------------------------\n";
        cout << "1.Shop\n2.Checkout\n3.Feedback/Complaint\n4.See Responses\n5.Return to login page\n6.Exit the program\n";
        cout << " -----------------------------------------\n";
    }
};

class FoodAndBeverages : public shop {
public:
    void show() override {
        loadItemsFromFile("FoodAndBeverages");
        const int widthItem = 25;  // Adjusted width for longer item names
        const int widthRate = 15; // Width for Rate column
        const int widthStock = 10; // Width for Stock column

        cout << "Food and Beverages:\n";
        cout << std::left
            << std::setw(widthItem) << "Item"
            << std::setw(widthRate) << "Rate (Rs.)"
            << std::setw(widthStock) << "Stock"
            << "\n";
        cout << std::string(widthItem + widthRate + widthStock, '-') << "\n";

        for (const auto& item : items["FoodAndBeverages"]) {
            cout << std::left
                << std::setw(widthItem) << item.first
                << std::setw(widthRate) << item.second
                << std::setw(widthStock) << stock["FoodAndBeverages"][item.first]
                << "\n";
        }
    
        
    }

    void select(customer& customer) override {
        string item;
        int quantity;
        cout << "Type 'return' to go back.\n";
        cout << "Enter item name to add to your cart: ";
        cin >> item;

        if (item == "return") {
            clearScreen();
            return;  
        }

        
        if (items["FoodAndBeverages"].find(item) != items["FoodAndBeverages"].end()) {
            cout << "Enter quantity: ";
            cin >> quantity;

           
            if (stock["FoodAndBeverages"][item] >= quantity) {
                customer.addToCart("FoodAndBeverages", item, quantity);
                
                stock["FoodAndBeverages"][item] -= quantity;
                updateStockToFile("FoodAndBeverages");
                cout << "Item added to your cart.\n";
            }
            else {
                
                cout << "Error: Not enough stock available. Only "
                    << stock["FoodAndBeverages"][item]
                    << " item(s) are available.\n";
            }
        }
        else {
            cout << "Item not found.\n";
        }
    }

};


class Clothes : public shop {
public:
    void show() override {
        loadItemsFromFile("Clothes");
        const int widthItem = 20;  // Width for Item column
        const int widthRate = 15;  // Width for Rate column
        const int widthStock = 10; // Width for Stock column

        cout << "Clothes:\n";
        cout << std::left
            << std::setw(widthItem) << "Item"
            << std::setw(widthRate) << "Rate (Rs.)"
            << std::setw(widthStock) << "Stock"
            << "\n";
        cout << std::string(widthItem + widthRate + widthStock, '-') << "\n";

        for (const auto& item : items["Clothes"]) {
            cout << std::left
                << std::setw(widthItem) << item.first
                << std::setw(widthRate) << item.second
                << std::setw(widthStock) << stock["Clothes"][item.first]
                << "\n";
        }
    }

    void select(customer& customer) override {
        string item;
        int quantity;
        cout << "Type 'return' to go back.\n";
        cout << "Enter item name to add to your cart: ";
        cin >> item;

        if (item == "return") {
            clearScreen();
            return;  // Exit the select function and return to the previous menu
        }

        // Check if the item exists in the "Clothes" category
        if (items["Clothes"].find(item) != items["Clothes"].end()) {
            cout << "Enter quantity: ";
            cin >> quantity;

            // Check if requested quantity is available in stock
            if (stock["Clothes"][item] >= quantity) {
                customer.addToCart("Clothes", item, quantity);
                // Decrease the stock after purchase
                stock["Clothes"][item] -= quantity;
                updateStockToFile("Clothes");
                cout << "Item added to your cart.\n";
            }
            else {
                // If requested quantity exceeds available stock
                cout << "Error: Not enough stock available. Only "
                    << stock["Clothes"][item]
                    << " item(s) are available.\n";
            }
        }
        else {
            cout << "Item not found.\n";
        }
    }

};


class Stationery : public shop {
public:
    void show() override {
        loadItemsFromFile("Stationery");
        const int widthItem = 20;
        const int widthRate = 15;
        const int widthStock = 10;

        cout << "Stationery:\n";
        cout << std::left
            << std::setw(widthItem) << "Item"
            << std::setw(widthRate) << "Rate (Rs.)"
            << std::setw(widthStock) << "Stock"
            << "\n";
        cout << std::string(widthItem + widthRate + widthStock, '-') << "\n";

        for (const auto& item : items["Stationery"]) {
            cout << std::left
                << std::setw(widthItem) << item.first
                << std::setw(widthRate) << item.second
                << std::setw(widthStock) << stock["Stationery"][item.first]
                << "\n";
        }
    }

    void select(customer& customer) override {
        string item;
        int quantity;
        cout << "Type 'return' to go back.\n";
        cout << "Enter item name to add to your cart: ";
        cin >> item;

        if (item == "return") {
            clearScreen();
            return;  // Exit the select function and return to the previous menu
        }

        // Check if the item exists in the "Stationery" category
        if (items["Stationery"].find(item) != items["Stationery"].end()) {
            cout << "Enter quantity: ";
            cin >> quantity;

            // Check if requested quantity is available in stock
            if (stock["Stationery"][item] >= quantity) {
                customer.addToCart("Stationery", item, quantity);
                // Decrease the stock after purchase
                stock["Stationery"][item] -= quantity;
                updateStockToFile("Stationery");
                cout << "Item added to your cart.\n";
            }
            else {
                // If requested quantity exceeds available stock
                cout << "Error: Not enough stock available. Only "
                    << stock["Stationery"][item]
                    << " item(s) are available.\n";
            }
        }
        else {
            cout << "Item not found.\n";
        }
    }


};

class Electronics : public shop {
public:
    void show() override {
        loadItemsFromFile("Electronics");
        const int widthItem = 20;
        const int widthRate = 15;
        const int widthStock = 10;

        cout << "Electronics:\n";
        cout << std::left
            << std::setw(widthItem) << "Item"
            << std::setw(widthRate) << "Rate (Rs.)"
            << std::setw(widthStock) << "Stock"
            << "\n";
        cout << std::string(widthItem + widthRate + widthStock, '-') << "\n";

        for (const auto& item : items["Electronics"]) {
            cout << std::left
                << std::setw(widthItem) << item.first
                << std::setw(widthRate) << item.second
                << std::setw(widthStock) << stock["Electronics"][item.first]
                << "\n";
        }
    }

    void select(customer& customer) override {
        string item;
        int quantity;
        cout << "Type 'return' to go back.\n";
        cout << "Enter item name to add to your cart: ";
        cin >> item;

        if (item == "return") {
            clearScreen();
            return;  // Exit the select function and return to the previous menu
        }

        // Check if the item exists in the "Electronics" category
        if (items["Electronics"].find(item) != items["Electronics"].end()) {
            cout << "Enter quantity: ";
            cin >> quantity;

            // Check if requested quantity is available in stock
            if (stock["Electronics"][item] >= quantity) {
                customer.addToCart("Electronics", item, quantity);
                // Decrease the stock after purchase
                stock["Electronics"][item] -= quantity;
                updateStockToFile("Electronics");
                cout << "Item added to your cart.\n";
            }
            else {
                // If requested quantity exceeds available stock
                cout << "Error: Not enough stock available. Only "
                    << stock["Electronics"][item]
                    << " item(s) are available.\n";
            }
        }
        else {
            cout << "Item not found.\n";
        }
    }

};

class Books : public shop {
public:
    void show() override {
        loadItemsFromFile("Books");
        const int widthItem = 20;
        const int widthRate = 15;
        const int widthStock = 10;

        cout << "Books:\n";
        cout << std::left
            << std::setw(widthItem) << "Item"
            << std::setw(widthRate) << "Rate (Rs.)"
            << std::setw(widthStock) << "Stock"
            << "\n";
        cout << std::string(widthItem + widthRate + widthStock, '-') << "\n";

        for (const auto& item : items["Books"]) {
            cout << std::left
                << std::setw(widthItem) << item.first
                << std::setw(widthRate) << item.second
                << std::setw(widthStock) << stock["Books"][item.first]
                << "\n";
        }
    }

    void select(customer& customer) override {
        string item;
        int quantity;

        // Allow user to type "return" to go back
        cout << "Type 'return' to go back.\n";
        cout << "Enter item name to add to your cart: ";
        cin >> item;

        // If user types "return", exit the function
        if (item == "return") {
            clearScreen();
            return;  // Exit the select function and return to the previous menu
        }

        if (items["Books"].find(item) != items["Books"].end()) {
            cout << "Enter quantity: ";
            cin >> quantity;

            // Check if requested quantity is available in stock
            if (stock["Books"][item] >= quantity) {
                customer.addToCart("Books", item, quantity);
                // Decrease the stock after purchase
                stock["Books"][item] -= quantity;
                updateStockToFile("Books");  // Update the stock in the file
                cout << "Item added to your cart.\n";
            }
            else {
                // If requested quantity exceeds available stock
                cout << "Error: Not enough stock available. Only "
                    << stock["Books"][item]
                    << " item(s) are available.\n";
            }
        }
        else {
            cout << "Item not found.\n";
        }
    }


};

bool Initialize() {
    WSADATA data;
    return WSAStartup(MAKEWORD(2, 2), &data) == 0;
}

void SendMsg(SOCKET s) {
    string message;
    cout << "Write your feedback or complaint below and press enter to send.\n";
    cout << "(Type 'menu' to return to the main menu or 'exit' to quit.)\n";

    while (true) {
        getline(cin, message);

        if (message == "menu") {
            cout << "Returning to the main menu...\n";
            break;  // Exit the loop and return to the menu
        }
        else if (message == "exit") {
            cout << "Exiting the program. Thank you for your feedback!\n";
            exit(0);  // Exit the program
        }

        string msgToSend = "Message from the customer: " + message;
        if (message.empty()) {
            cout << "Empty message entered. Not sending.\n";
            continue;
        }

        int bytesent = send(s, msgToSend.c_str(), msgToSend.length(), 0);
        if (bytesent == SOCKET_ERROR) {
            cout << "Error sending message.\n";
            break;
        }
        cout << "Message sent successfully.\n";
    }

    closesocket(s);
    WSACleanup();

}




void ReceiveMsg(SOCKET s) {
    char buffer[4096];
    int recvlength;
    string msg = "";
    while (1) {
        recvlength = recv(s, buffer, sizeof(buffer), 0);
        if (recvlength <= 0) {
            cout << "disconnected from the server" << endl;
            break;
        }
        else {
            msg = string(buffer, recvlength);
            cout << msg << endl;
        }
    }
}

void complaint() {
    if (!Initialize()) {
        cout << "Initialize winsock failed" << endl;

        return ;
    }
    SOCKET s;
    s = socket(AF_INET, SOCK_STREAM, 0);
    if (s == INVALID_SOCKET) {
        cout << "invalid socket created" << endl;
        return ;
    }

    int port = 12345;
    string serveraddress = "127.0.0.1";
    sockaddr_in serveraddr;
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(port);
    inet_pton(AF_INET, serveraddress.c_str(), &(serveraddr.sin_addr));

    if (connect(s, reinterpret_cast <sockaddr*>(&serveraddr), sizeof(serveraddr)) == SOCKET_ERROR) {
        cout << "not able to connect to server" << endl;
        closesocket(s);
        WSACleanup();
        return ;
    }

    cout << "Welcome to feedback/complaint section." << endl;

    thread senderthread(SendMsg, s);
    thread receiver(ReceiveMsg, s);

    senderthread.join();
    receiver.join();
 
    
}


 
void viewFeedbackHistory() {
    const string chatHistoryFile = "C:\\Users\\dsami\\source\\repos\\server\\server\\customer_messages.txt";

    while (true) {
        ifstream infile(chatHistoryFile);
        if (!infile.is_open()) {
            cout << "No feedback history found.\n";
            return;
        }

        // Display all feedback
        string line;
        cout << "\nFeedback History:\n";
        while (getline(infile, line)) {
            cout << line << endl;
        }
        infile.close();

        cout << "\nEnter the Serial Number (S.N) of the feedback you want to reply to, or type 'menu' to return to the menu: ";
        string input;
        cin >> input;

        if (input == "menu") {
            cout << "Returning to the menu...\n";
            clearScreen();
            return;
        }

        int serialNumber;
        try {
            serialNumber = stoi(input);
        }
        catch (...) {
            cout << "Invalid input. Please enter a valid serial number or type 'menu'.\n";
            continue;
        }

        infile.open(chatHistoryFile);
        if (!infile.is_open()) {
            cout << "Error reopening feedback history file.\n";
            return;
        }

        bool found = false;
        string selectedFeedback;
        while (getline(infile, line)) {
            if (line.find(to_string(serialNumber) + ".") != string::npos) {
                selectedFeedback = line;
                found = true;
                break;
            }
        }
        infile.close();

        if (!found) {
            cout << "No feedback found with Serial Number " << serialNumber << ".\n";
            continue;
        }

        cout << "\nSelected Feedback:\n" << selectedFeedback << endl;
        cout << "Enter your reply: ";
        cin.ignore();
        string reply;
        getline(cin, reply);

        // Sending the reply to the server
        if (!Initialize()) {
            cout << "Failed to initialize Winsock. Reply not sent.\n";
            return;
        }

        SOCKET s = socket(AF_INET, SOCK_STREAM, 0);
        if (s == INVALID_SOCKET) {
            cout << "Failed to create socket. Reply not sent.\n";
            WSACleanup();
            return;
        }

        const int port = 12345;
        const string serverAddress = "127.0.0.1";

        sockaddr_in serverAddr = {};
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_port = htons(port);
        inet_pton(AF_INET, serverAddress.c_str(), &serverAddr.sin_addr);

        if (connect(s, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
            cout << "Failed to connect to server. Reply not sent.\n";
            closesocket(s);
            WSACleanup();
            clearScreen();
            return;
        }

        string fullReply = "Reply to Feedback " + to_string(serialNumber) + ": " + reply;
        if (send(s, fullReply.c_str(), fullReply.size(), 0) == SOCKET_ERROR) {
            cout << "Failed to send reply to the server.\n";
            clearScreen();
        }
        else {
            cout << "Reply sent successfully.\n";
        }

        closesocket(s);
        WSACleanup();
    }
}


void checkFeedbackComplaints() {
    cout << "\nFeedback/Complaints:\n";
    viewFeedbackHistory();
}



void seeResponses() {
    ifstream feedbackFile("C:\\Users\\dsami\\source\\repos\\server\\server\\customer_messages.txt");
    ifstream responseFile("C:\\Users\\dsami\\source\\repos\\server\\server\\seller_responses.txt");

    if (!feedbackFile.is_open()) {
        cerr << "Error: Unable to open customer_messages.txt at the specified path." << endl;
    }
    if (!responseFile.is_open()) {
        cerr << "Error: Unable to open seller_responses.txt at the specified path." << endl;
    }


    string feedbackLine, responseLine;
    vector<string> responses;

    // Read all responses into a vector for easy searching
    while (getline(responseFile, responseLine)) {
        responses.push_back(responseLine);
    }

    // Process each feedback line by line
    while (getline(feedbackFile, feedbackLine)) {
        // Extract the serial number from the feedback
        size_t pos = feedbackLine.find(". ");
        if (pos != string::npos) {
            try {
                int serialNumber = stoi(feedbackLine.substr(0, pos));

                // Display the feedback
                cout << "Feedback #" << serialNumber << ": " << feedbackLine.substr(pos + 2) << endl;

                // Search for a corresponding response
                bool responseFound = false;
                for (const auto& response : responses) {
                    string searchString = "Reply to Feedback " + to_string(serialNumber) + ":";
                    if (response.find(searchString) != string::npos) {
                        // Extract and display the actual response message
                        cout << "Response: " << response.substr(response.find(":") + 2) << endl;
                        responseFound = true;
                        break;
                    }
                }

                // If no response is found, indicate it
                if (!responseFound) {
                    cout << "Response: This feedback/complaint is not responded." << endl;
                }

            }
            catch (...) {
                cout << "Error processing feedback line: " << feedbackLine << endl;
                continue;
            }
        }
    }

    feedbackFile.close();
    responseFile.close();
}

void checkManageStocks() {
    vector<string> categories = {
        "FoodAndBeverages",
        "Clothes",
        "Stationery",
        "Electronics",
        "Books"
    };

    for (const auto& category : categories) {
        string fileName = category + ".txt";

        // Read items from the file
        ifstream infile(fileName);
        if (!infile.is_open()) {
            cout << "Error: Could not open " << fileName << "\n";
            continue;
        }

        map<string, pair<double, int>> categoryItems; // Store item name, price, and stock
        string itemName;
        double price;
        int stock;

        cout << "\nManaging stocks for category: " << category << "\n";
        cout << "-------------------------------------------\n";
        cout << left << setw(20) << "Item Name" << setw(15) << "Price (Rs.)" << setw(10) << "Stock" << "\n";
        cout << "-------------------------------------------\n";

        while (infile >> itemName >> price >> stock) {
            categoryItems[itemName] = { price, stock };
            cout << left << setw(20) << itemName
                << setw(15) << price
                << setw(10) << stock << "\n";
        }
        infile.close();

        // Menu for managing stocks and prices
        while (true) {
            cout << "\nOptions:\n";
            cout << "1. Edit stock\n";
            cout << "2. Edit price\n";
            cout << "3. Add new item\n";
            cout << "4. Go to next category\n";
            cout << "5. Return to the menu\n";
            cout << "Enter your choice: ";
            int choice;
            cin >> choice;

            if (choice == 5) {
                clearScreen();
                cout << "Returning to the main menu...\n";
                return;
            }

            if (choice == 4) break;

          

 

            switch (choice) {
            case 1: { // Edit stock
                cout << "Enter item name: ";
                cin >> ws;
                getline(cin, itemName);

                if (categoryItems.find(itemName) == categoryItems.end()) {
                    cout << "Error: Item \"" << itemName << "\" not found.\n";
                    continue;
                }
                cout << "Current stock: " << categoryItems[itemName].second << "\n";
                cout << "Enter new stock: ";
                cin >> stock;
                categoryItems[itemName].second = stock;
                break;
            }
            case 2: { // Edit price\
                                           cout << "Enter item name: ";
                cin >> ws;
                getline(cin, itemName);

                if (categoryItems.find(itemName) == categoryItems.end()) {
                    cout << "Error: Item \"" << itemName << "\" not found.\n";
                    continue;
                }
                cout << "Current price: Rs." << categoryItems[itemName].first << "\n";
                cout << "Enter new price: Rs.";
                cin >> price;
                categoryItems[itemName].first = price;
                break;
            }


            case 3: {
                cout << "Enter new item name: ";
                cin >> ws;  // Ensures any leading whitespace is ignored
                getline(cin, itemName);  // Correctly take input for the new item name

                if (categoryItems.find(itemName) != categoryItems.end()) {
                    cout << "Error: Item \"" << itemName << "\" already exists.\n";
                    continue;
                }

                cout << "Enter price (Rs.): ";
                while (!(cin >> price) || price < 0) {
                    cout << "Invalid input. Please enter a valid price: ";
                    cin >> price;
                }

                cout << "Enter stock: ";
                while (!(cin >> stock) || stock < 0) {
                    cout << "Invalid input. Please enter a valid stock quantity: ";
                    cin >> stock;
                }

                categoryItems[itemName] = { price, stock };

                ofstream outfile(fileName, ios::app); // Append mode
                if (!outfile.is_open()) {
                    cout << "Error: Could not write to " << fileName << "\n";
                    continue;
                }
                outfile << itemName << " " << price << " " << stock << "\n";
                outfile.close();

                cout << "Item \"" << itemName << "\" added successfully.\n";
                continue;
            }

            default:
                cout << "Invalid choice. Try again.\n";
            }

            ofstream outfile(fileName);
            if (!outfile.is_open()) {
                cout << "Error: Could not write to " << fileName << "\n";
                continue;
            }

            for (const auto& entry : categoryItems) {
                outfile << entry.first << " " << entry.second.first << " " << entry.second.second << "\n";
            }
            outfile.close();
            cout << "Changes saved for " << category << ".\n";
        }
    }
    cout << "\nStock management complete.\n";
}





        


void sellermenu() {
    while (true) {
        cout << "\nSeller Menu:\n";
        cout << "1. Check/Manage Stocks\n";
        cout << "2. Check Feedback/Complaints\n";
        cout << "3. View Transaction History\n";
        cout << "4. Return Login page\n";
        cout << "5. Exit the program\n";
        cout << "Enter your choice: ";
        int choice;
        cin >> choice;

        switch (choice) {
        case 1:
            clearScreen();
            checkManageStocks();
            break;
        case 2:
            clearScreen();
            checkFeedbackComplaints();
            break;
        case 3:
            clearScreen();
            cout << "Transaction History" << endl;
            viewTransactionHistory();
            break;
        case 4:
            clearScreen();
            cout << "Returning to login page..\n";
            return;
        case 5:
            clearScreen();
            cout << "Exiting.. ";
            exit(0);
        default:
            cout << "Invalid choice. Please try again.\n";
        }
    }
}

void selectCategory() {
    customer myCustomer;

    shop* shop = nullptr;

    while (true) {
        cout << "\nSelect Category:\n";
        cout << "1. Food and Beverages\n";
        cout << "2. Clothes\n";
        cout << "3. Stationery\n";
        cout << "4. Electronics\n";
        cout << "5. Books\n";
        cout << "6. View Cart\n";
        cout << "7. Return to Menu\n";
        cout << "Enter your choice:";
        int choice;
        cin >> choice;

        switch (choice) {
        case 1: {
            clearScreen();
            
            FoodAndBeverages foodShop;
            shop = &foodShop;
            shop->show();
            shop->select(myCustomer);
            break;
        }
        case 2: {
            clearScreen();
            Clothes clothesShop;
            shop = &clothesShop;
            shop->show();
            shop->select(myCustomer);
            break;
        }
        case 3: {
            clearScreen();
            Stationery stationeryShop;
            shop = &stationeryShop;
            shop->show();
            shop->select(myCustomer);
            break;
        }
        case 4: {
            clearScreen();
            Electronics electronicsShop;
            shop = &electronicsShop;
            shop->show();
            shop->select(myCustomer);
            break;
        }
        case 5: {
            clearScreen();
            Books booksShop;
            shop = &booksShop;
            shop->show();
            shop->select(myCustomer);
            break;
        }
        case 6:
            clearScreen();
            myCustomer.viewCart();
            break;
        case 7:
            clearScreen();
            cout << "Returning to menu...\n";
            return ;
        default:
            clearScreen();
            cout << "Invalid option. Please try again.\n";
        }


    }
}

void login() {
    while (true) {
        cout << "Login as:\n";
        cout << "1. Seller\n2. Buyer Login\n3. Buyer Signup\n4. Exit\n";
        cout << "Enter your choice:" << endl;
        int choice;
        cin >> choice;

        if (choice == 1) {
            clearScreen();
            if (sellerLogin()) {
                sellermenu();
            }
        }
        else if (choice == 2) {
            clearScreen();
            string username;
            if (buyerLogin(username)) {
          
                customer c;

                shop* s = nullptr;
                while (true) {
                    if (!s) {
                        shop temp;
                        temp.showMenu();
                    }

                    cout << "Select an option (1-6): ";
                    int val;
                    cin >> val;

                    if (val == 1) {
                        clearScreen();
                        selectCategory();
                    }
                    
                    else if (val == 3) {
                        clearScreen();
                        complaint();
                    }
                    else if (val == 4) {
                        clearScreen();
                        seeResponses();
                    }
                    else if (val == 5) {
                        clearScreen();
                        cout << "Returning to Login Page...\n";
                        login();

                    }
                    else if (val == 6) {
                        return;
                    }
                    else if (val == 2) {
                        clearScreen();
                        c.printBill();

                        // Offer a choice after billing
                        cout << "What would you like to do next?\n";
                        cout << "1. Return to Menu\n2. Exit Program\n";
                        int postBillChoice;
                        cin >> postBillChoice;

                        if (postBillChoice == 1) {
                            clearScreen();
                            continue; // Return to menu
                        }
                        else if (postBillChoice == 2) {
                            clearScreen();
                            cout << "Exiting the program. Thank you for shopping!\n";
                            return; // Exit the program
                        }
                        else {
                            clearScreen();
                            cout << "Invalid choice. Exiting by default.\n";
                            return;
                        }
                    }
                    else {
                        cout << "Invalid choice. Please try again.\n";
                    }
                }
                return;
            }
        }
        else if (choice == 3) {
            clearScreen();
            buyerSignup();
        }
        else if (choice == 4) {
            clearScreen();
            cout << "Exiting...\n";
            return;
        }
        else {
            cout << "Invalid choice. Please try again.\n";
        }
    }
};


int main() {
    cout << "Welcome to SMARTSHOP" << endl << endl;

    loadCredentials();
    login();

   
    WSACleanup();


	return 0;
}