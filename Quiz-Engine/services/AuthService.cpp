#include "AuthService.h"
#include <iostream>

using namespace std;

AuthService::AuthService(DatabaseStorage& storage) : storage(storage) {}

int AuthService::authenticate() {
    int choice;
    cout << "\n--- STUDENT AUTH ---" << endl;
    cout << "1. Login\n2. Register\nChoice: ";
    cin >> choice;

    string name, email, password;

    if (choice == 1) {
        cout << "Enter Email: ";
        cin >> email;
        cout << "Enter Password: ";
        cin >> password;

        int student_id = storage.loginStudent(email, password);

        if (student_id != -1) {
            cout << "\nLogin successful!\n";
            return student_id;
        }
        else {
            cout << "\n[!] Invalid credentials!\n";
            return -1;
        }
    }
    else if (choice == 2) {
        cout << "Enter Name: ";
        cin >> name;
        cout << "Enter Email: ";
        cin >> email;
        cout << "Enter Password: ";
        cin >> password;

        int student_id = storage.registerStudent(name, email, password);

        if (student_id != -1) {
            cout << "\nRegistration successful! You are now logged in.\n";
            return student_id;
        }
        else {
            cout << "\n[!] Registration failed. Email might already be in use.\n";
            return -1;
        }
    }

    return -1;
}