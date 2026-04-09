#include <iostream>
#include <string>
#include <exception>

// Persistence
#include "persistence/DatabaseStorage.h"

// Services
#include "services/QuizEngine.h"
#include "services/AuthService.h"
#include "services/AdminService.h"
#include "services/StudentService.h"

using namespace std;

int main() {
    try {
        // Instantiate our core dependencies once
        DatabaseStorage storage;
        QuizEngine engine;
        AuthService auth(storage);

        int roleChoice;
        bool running = true;

        while (running) {
            cout << "\n====================================" << endl;
            cout << "       WELCOME TO QUIZ PLATFORM     " << endl;
            cout << "====================================" << endl;
            cout << "1. Admin Login" << endl;
            cout << "2. Student Login / Register" << endl;
            cout << "3. Exit" << endl;
            cout << "Select Option: ";
            cin >> roleChoice;

            if (roleChoice == 1) {
                string adminPass;
                cout << "Enter Admin Password: ";
                cin >> adminPass;

                if (adminPass == "admin123") {
                    // Instantiate AdminService and pass the DB connection
                    AdminService admin(storage);
                    admin.run();
                }
                else {
                    cout << "\n[!] Access Denied: Incorrect Admin Password!\n";
                }
            }
            else if (roleChoice == 2) {
                // Route through AuthService
                int student_id = auth.authenticate();

                if (student_id != -1) {
                    // Instantiate StudentService and pass DB and Engine
                    StudentService student(engine, storage);
                    student.run(student_id);
                }
            }
            else if (roleChoice == 3) {
                cout << "Exiting system. Goodbye!" << endl;
                running = false;
            }
            else {
                cout << "Invalid option. Please try again." << endl;
            }
        }
    }
    catch (const exception& e) {
        cerr << "\n[!] System Error: " << e.what() << endl;
        return 1;
    }

    return 0;
}