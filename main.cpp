#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>

using namespace std;

// Product structure
struct Product {
    int id;
    string name;
    string category;
    double price;
};

// Demo product list
vector<Product> products = {
    {1, "Floral Kurti", "Kurtis", 799},
    {2, "Silk Saree", "Sarees", 1499},
    {3, "Makeup Kit", "Makeup", 999},
    {4, "Gaming Laptop", "Laptops", 55999},
    {5, "Smartphone", "Mobiles", 18999},
    {6, "Party Wear Kurti", "Kurtis", 899}
};

// Demo users
unordered_map<string, string> users = {
    {"kanya725", "1234"},
    {"admin", "admin123"}
};

// Login function
bool login(string username, string password) {

    if (users.find(username) != users.end()) {
        if (users[username] == password) {
            return true;
        }
    }

    return false;
}

// Display products
void displayProducts() {

    cout << "\n========== KANYAMART PRODUCTS ==========\n";

    for (const auto& product : products) {

        cout << "\nID       : " << product.id;
        cout << "\nName     : " << product.name;
        cout << "\nCategory : " << product.category;
        cout << "\nPrice    : Rs." << product.price;
        cout << "\n---------------------------------------------";
    }
}

// Search products
void searchProducts(string keyword) {

    bool found = false;

    cout << "\n========== SEARCH RESULTS ==========\n";

    for (const auto& product : products) {

        if (product.name.find(keyword) != string::npos ||
            product.category.find(keyword) != string::npos) {

            cout << "\n" << product.id
                 << " - " << product.name
                 << " - Rs." << product.price;

            found = true;
        }
    }

    if (!found) {
        cout << "\nNo products found.";
    }

    cout << endl;
}

// Main program
int main() {

    string username;
    string password;

    cout << "=====================================\n";
    cout << "              KANYAMART\n";
    cout << "=====================================\n";

    cout << "\nUsername: ";
    cin >> username;

    cout << "Password: ";
    cin >> password;

    // Login validation
    if (!login(username, password)) {

        cout << "\nInvalid username or password!\n";
        return 0;
    }

    cout << "\nLogin successful!";
    cout << "\nWelcome to KanyaMart!\n";

    int choice;

    do {

        cout << "\n\n========== MENU ==========\n";
        cout << "1. Display Products\n";
        cout << "2. Search Products\n";
        cout << "3. Logout\n";
        cout << "Enter your choice: ";

        cin >> choice;

        switch (choice) {

            case 1:
                displayProducts();
                break;

            case 2: {
                string keyword;

                cout << "Enter product/category: ";
                cin >> keyword;

                searchProducts(keyword);
                break;
            }

            case 3:
                cout << "\nLogged out successfully!\n";
                break;

            default:
                cout << "\nInvalid choice!";
        }

    } while (choice != 3);

    return 0;
}
