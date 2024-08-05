#include <iostream>
#include <fstream>
#include <string>
#include <vector>

using namespace std;

class User {
protected:
    string username;
    string password;
public:
    User() {}
    User(string u, string p) : username(u), password(p) {}
    virtual void viewExpenditures() const = 0;
    string getUsername() const { return username; }
    string getPassword() const { return password; }
    virtual ~User() {}
};

class RegularUser : public User {
public:
    RegularUser(string u, string p) : User(u, p) {}

    void viewExpenditures() const override {
        ifstream userFile(username + ".txt");
        if (!userFile.is_open()) {
            cout << "Could not open user file.\n";
            return;
        }
        string line;
        while (getline(userFile, line)) {
            cout << line << "\n";
        }
        userFile.close();
    }

    void viewTotalExpenditures() const {
        ifstream file("total_expenditures.txt");
        if (!file.is_open()) {
            cout << "Could not open total expenditures file.\n";
            return;
        }
        string line;
        while (getline(file, line)) {
            cout << line << "\n";
        }
        file.close();
    }
};

class Admin : public User {
public:
    Admin(string u, string p) : User(u, p) {}

    void addUser(string u, string p) {
        ofstream userFile(u + ".txt");
        userFile << "Username: " << u << "\n";
        userFile << "Password: " << p << "\n";
        userFile.close();
        cout << "User added successfully.\n";
    }

    void removeUser(string u) {
        if (remove((u + ".txt").c_str()) == 0) {
            cout << "User removed successfully.\n";
        } else {
            cout << "Error removing user.\n";
        }
    }

    void viewExpenditures() const override {
        ifstream file("total_expenditures.txt");
        if (!file.is_open()) {
            cout << "Could not open total expenditures file.\n";
            return;
        }
        string line;
        while (getline(file, line)) {
            cout << line << "\n";
        }
        file.close();
    }
};

class UserManager {
    vector<User*> users;
public:
    void addUser(User* user) {
        users.push_back(user);
    }

    User* authenticateUser(const string& username, const string& password) {
        for (User* user : users) {
            if (user->getUsername() == username && user->getPassword() == password) {
                return user;
            }
        }
        return nullptr;
    }

    ~UserManager() {
        for (User* user : users) {
            delete user;
        }
    }
};

void showAdminMenu() {
    cout << "1. Add User\n";
    cout << "2. Remove User\n";
    cout << "3. View Total Expenditures\n";
    cout << "4. Exit\n";
    cout << "Enter your choice: ";
}

void showUserMenu() {
    cout << "1. View Personal Expenditures\n";
    cout << "2. View Total Expenditures\n";
    cout << "3. Exit\n";
    cout << "Enter your choice: ";
}

int main() {
    UserManager userManager;
    userManager.addUser(new Admin("admin", "adminpass"));
    userManager.addUser(new RegularUser("user1", "user1pass"));
    userManager.addUser(new RegularUser("user2", "user2pass"));

    int choice;
    while (true) {
        cout << "1. Admin Portal\n";
        cout << "2. User Portal\n";
        cout << "3. Exit\n";
        cout << "Enter your choice: ";
        cin >> choice;

        if (choice == 1) {
            string username, password;
            cout << "Enter Username: ";
            cin >> username;
            cout << "Enter Password: ";
            cin >> password;
            User* user = userManager.authenticateUser(username, password);
            Admin* admin = dynamic_cast<Admin*>(user);
            if (admin) {
                while (true) {
                    showAdminMenu();
                    int adminChoice;
                    cin >> adminChoice;
                    if (adminChoice == 1) {
                        string u, p;
                        cout << "Enter new Username: ";
                        cin >> u;
                        cout << "Enter new Password: ";
                        cin >> p;
                        admin->addUser(u, p);
                    } else if (adminChoice == 2) {
                        string u;
                        cout << "Enter Username to Remove: ";
                        cin >> u;
                        admin->removeUser(u);
                    } else if (adminChoice == 3) {
                        admin->viewExpenditures();
                    } else if (adminChoice == 4) {
                        break;
                    } else {
                        cout << "Invalid choice. Try again.\n";
                    }
                }
            } else {
                cout << "Authentication failed. Try again.\n";
            }
        } else if (choice == 2) {
            string username, password;
            cout << "Enter Username: ";
            cin >> username;
            cout << "Enter Password: ";
            cin >> password;
            User* user = userManager.authenticateUser(username, password);
            RegularUser* regularUser = dynamic_cast<RegularUser*>(user);
            if (regularUser) {
                while (true) {
                    showUserMenu();
                    int userChoice;
                    cin >> userChoice;
                    if (userChoice == 1) {
                        regularUser->viewExpenditures();
                    } else if (userChoice == 2) {
                        regularUser->viewTotalExpenditures();
                    } else if (userChoice == 3) {
                        break;
                    } else {
                        cout << "Invalid choice. Try again.\n";
                    }
                }
            } else {
                cout << "Authentication failed. Try again.\n";
            }
        } else if (choice == 3) {
            break;
        } else {
            cout << "Invalid choice. Try again.\n";
        }
    }
    return 0;
}
