#include "AdminService.h"
#include <iostream>

using namespace std;

AdminService::AdminService(DatabaseStorage& storage) : storage(storage) {}

void AdminService::run() {
    int choice;
    bool active = true;

    while (active) {
        cout << "\n--- ADMIN PANEL ---" << endl;
        cout << "1. Add Subject" << endl;
        cout << "2. Add Question" << endl;
        cout << "3. View Questions" << endl;
        cout << "4. Logout" << endl;
        cout << "Choice: ";
        cin >> choice;

        if (choice == 1) {
            storage.addSubject();
        }
        else if (choice == 2) {
            storage.addQuestion();
        }
        else if (choice == 3) {
            storage.viewQuestions();
        }
        else if (choice == 4) {
            cout << "Logging out of admin...\n";
            active = false;
        }
        else {
            cout << "Invalid choice.\n";
        }
    }
}