#include "StudentService.h"
#include <iostream>

using namespace std;

StudentService::StudentService(QuizEngine& engine, DatabaseStorage& storage)
    : engine(engine), storage(storage) {
}

void StudentService::run(int student_id) {
    int choice;
    bool active = true;
    while (active) {
        cout << "\n--- STUDENT DASHBOARD ---" << endl;
        cout << "1. Take Adaptive Quiz" << endl;
        cout << "2. View Past Attempts" << endl;
        cout << "3. Logout" << endl;
        cout << "Choice: ";
        cin >> choice;

        if (choice == 1) {
            engine.runQuiz(student_id);
        }
        else if (choice == 2) {
            storage.viewResults(student_id);
        }
        else if (choice == 3) {
            cout << "Logging out...\n";
            active = false;
        }
        else {
            cout << "Invalid choice.\n";
        }
    }
}