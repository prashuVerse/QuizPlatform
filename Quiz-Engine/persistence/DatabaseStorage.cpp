// This is DAL file responsible for talking with MYSQL
#include <iostream>
#include <vector>
#include <random>
#include <algorithm>
#include <string>
#include <sstream> 
#include "../core/question.h"
#include "DatabaseStorage.h"

using namespace std;

#include <mysql/jdbc.h> // Mysql connector
#include <jdbc/cppconn/prepared_statement.h>

DatabaseStorage::DatabaseStorage() {}

sql::Connection* DatabaseStorage::connectTodatabase() {
    try {
        sql::Driver* driver = get_driver_instance();
        sql::Connection* con = driver->connect("tcp://127.0.0.1:3306", "root", "Rashmi@123");
        con->setSchema("quiz_engine");
        return con;
    }
    catch (sql::SQLException& e) {
        std::cerr << "Database Connection Error: " << e.what() << std::endl;
        return nullptr;
    }
}

// Function to load ALL questions from the database
vector<Question> DatabaseStorage::loadQuestions() {
    vector<Question> questionsList;
    sql::Connection* con = connectTodatabase();

    if (con == nullptr) {
        cout << "Failed to connect to database: No connection Found" << endl;
        return questionsList;
    }

    try {
        sql::Statement* stmt = con->createStatement();
        sql::ResultSet* res = stmt->executeQuery(
            "SELECT q.quest_id, s.subject_name, q.difficulty, q.quest_txt "
            "FROM Questions q JOIN Subject s ON q.sub_id = s.sub_id"
        );

        while (res->next()) {
            string qid = res->getString("quest_id");
            string qSubject = res->getString("subject_name");
            int qDifficulty = res->getInt("difficulty");
            string qText = res->getString("quest_txt");

            vector<string> optionList;
            vector<int> optionIds;
            int cOptId = -1;

            sql::PreparedStatement* optStmt = con->prepareStatement(
                "SELECT opt_id, option_txt, is_correct FROM Options WHERE quest_id = ?"
            );
            optStmt->setInt(1, stoi(qid));
            sql::ResultSet* optRes = optStmt->executeQuery();

            while (optRes->next()) {
                int optId = optRes->getInt("opt_id");
                optionList.push_back(optRes->getString("option_txt"));
                optionIds.push_back(optId);

                if (optRes->getBoolean("is_correct")) {
                    cOptId = optId;
                }
            }
            delete optRes;
            delete optStmt;

            Question newQuestion(qid, qSubject, qDifficulty, qText, optionList, optionIds, cOptId);
            questionsList.push_back(newQuestion);
        }
        delete res;
        delete stmt;
        delete con;
    }
    catch (sql::SQLException& e) {
        cerr << "SQL Error: " << e.what() << endl;
        delete con;
    }
    return questionsList;
}

// Function to load questions filtered by Subject and amount
vector<Question> DatabaseStorage::loadQuestions(string subject, int no_of_questions) {
    vector<Question> questionsList;
    sql::Connection* con = connectTodatabase();

    if (con == nullptr) {
        cout << "Failed to connect to database: No connection Found" << endl;
        return questionsList;
    }

    try {
        sql::PreparedStatement* pstmt = con->prepareStatement(
            "SELECT q.quest_id, s.subject_name, q.difficulty, q.quest_txt "
            "FROM Questions q "
            "JOIN Subject s ON q.sub_id = s.sub_id "
            "WHERE s.subject_name = ?"
        );
        pstmt->setString(1, subject);
        sql::ResultSet* res = pstmt->executeQuery();

        while (res->next()) {
            string qid = res->getString("quest_id");
            string qSubject = res->getString("subject_name");
            int qDifficulty = res->getInt("difficulty");
            string qText = res->getString("quest_txt");

            vector<string> optionList;
            vector<int> optionIds;
            int cOptId = -1;

            sql::PreparedStatement* optStmt = con->prepareStatement(
                "SELECT opt_id, option_txt, is_correct FROM Options WHERE quest_id = ?"
            );
            optStmt->setInt(1, stoi(qid));
            sql::ResultSet* optRes = optStmt->executeQuery();

            while (optRes->next()) {
                int optId = optRes->getInt("opt_id");
                optionList.push_back(optRes->getString("option_txt"));
                optionIds.push_back(optId);

                if (optRes->getBoolean("is_correct")) {
                    cOptId = optId;
                }
            }
            delete optRes;
            delete optStmt;

            Question newQuestion(qid, qSubject, qDifficulty, qText, optionList, optionIds, cOptId);
            questionsList.push_back(newQuestion);
        }
        delete res;
        delete pstmt;
        delete con;
    }
    catch (sql::SQLException& e) {
        cerr << "SQL Error: " << e.what() << endl;
        delete con;
    }

    random_device rd;
    mt19937 g(rd());
    shuffle(questionsList.begin(), questionsList.end(), g);

    vector<Question> SubjectWiseQuestionList;
    for (int i = 0; i < no_of_questions && i < questionsList.size(); i++) {
        SubjectWiseQuestionList.push_back(questionsList[i]);
    }
    return SubjectWiseQuestionList;
}

int DatabaseStorage::getSubjectId(string subjectName)  {
    // Note: To fix the 'const' error from the header, we must cast away constness 
    // just for the connection, or remove 'const' from the header. 
    // Assuming the header has 'const', we'll use a const_cast for the DB connection.
    sql::Connection* con = const_cast<DatabaseStorage*>(this)->connectTodatabase();
    if (!con) return -1;

    int subId = -1;
    try {
        sql::PreparedStatement* pstmt = con->prepareStatement(
            "SELECT sub_id FROM Subject WHERE subject_name = ?"
        );
        pstmt->setString(1, subjectName);
        sql::ResultSet* res = pstmt->executeQuery();

        if (res->next()) {
            subId = res->getInt("sub_id");
        }

        delete res;
        delete pstmt;
        delete con;
    }
    catch (sql::SQLException& e) {
        cerr << "SQL Error finding subject: " << e.what() << endl;
        delete con;
    }
    return subId;
}

// ---------------------------------------------------------
// AUTHENTICATION QUERIES (UPDATED FOR NEW SCHEMA)
// ---------------------------------------------------------
int DatabaseStorage::registerStudent(string name, string email, string password) {
    sql::Connection* con = connectTodatabase();
    if (!con) {
        cout << "Failed to connect to database." << endl;
        return -1;
    }

    try {
        // 1. Insert into USERS table (Notice email_id and 'Student' role)
        sql::PreparedStatement* pstmtUser = con->prepareStatement(
            "INSERT INTO USERS(name, email_id, password, role) VALUES (?, ?, ?, 'Student')"
        );
        pstmtUser->setString(1, name);
        pstmtUser->setString(2, email);
        pstmtUser->setString(3, password);
        pstmtUser->executeUpdate();
        delete pstmtUser;

        // 2. Fetch the generated user_id
        sql::Statement* stmt = con->createStatement();
        sql::ResultSet* res = stmt->executeQuery("SELECT LAST_INSERT_ID() AS user_id");

        int userId = -1;
        if (res->next()) {
            userId = res->getInt("user_id");
        }
        delete res;
        delete stmt;

        if (userId == -1) {
            delete con;
            return -1;
        }

        // 3. Insert into Student table
        sql::PreparedStatement* pstmtStudent = con->prepareStatement(
            "INSERT INTO Student(user_id) VALUES (?)"
        );
        pstmtStudent->setInt(1, userId);
        pstmtStudent->executeUpdate();
        delete pstmtStudent;

        // 4. Fetch the generated student_id
        stmt = con->createStatement();
        res = stmt->executeQuery("SELECT LAST_INSERT_ID() AS student_id");

        int studentId = -1;
        if (res->next()) {
            studentId = res->getInt("student_id");
        }
        delete res;
        delete stmt;
        delete con;

        return studentId;

    }
    catch (sql::SQLException& e) {
        cerr << "\n[Database Error] Registration failed: " << e.what() << endl;
        delete con;
        return -1;
    }
}

int DatabaseStorage::loginStudent(string email, string password) {
    sql::Connection* con = connectTodatabase();
    if (!con) {
        cout << "Failed to connect to database." << endl;
        return -1;
    }

    try {
        // Query checks email_id and returns the specific student_id
        sql::PreparedStatement* pstmt = con->prepareStatement(
            "SELECT s.student_id FROM Student s "
            "JOIN USERS u ON s.user_id = u.user_id "
            "WHERE u.email_id = ? AND u.password = ?"
        );
        pstmt->setString(1, email);
        pstmt->setString(2, password);

        sql::ResultSet* res = pstmt->executeQuery();

        int studentId = -1;
        if (res->next()) {
            studentId = res->getInt("student_id");
        }

        delete res;
        delete pstmt;
        delete con;

        return studentId;

    }
    catch (sql::SQLException& e) {
        cerr << "\n[Database Error] Login failed: " << e.what() << endl;
        delete con;
        return -1;
    }
}

// ---------------------------------------------------------
// RESULT SAVING QUERIES (UPDATED FOR COMPOSITE KEYS)
// ---------------------------------------------------------
void DatabaseStorage::saveAttempt(const Attempt& attempt, int student_id, int sub_id, int score) {
    sql::Connection* con = connectTodatabase();
    if (!con) {
        cout << "Failed to connect to database." << endl;
        return;
    }

    try {
        // 1. Insert into Personal_Test (Composite PK prevents duplicates naturally)
        sql::PreparedStatement* pstmt = con->prepareStatement(
            "INSERT IGNORE INTO Personal_Test(student_id, sub_id) VALUES (?, ?)"
        );
        pstmt->setInt(1, student_id);
        pstmt->setInt(2, sub_id);
        pstmt->execute();
        delete pstmt;

        const vector<int>& answer = attempt.getAllAnswers();
        const vector<Question>& questionsList = attempt.getQuiz().getQuestion();

        for (int i = 0; i < questionsList.size(); i++) {

            // 2. Insert into Personal_Test_Qs
            sql::PreparedStatement* pstmtQ = con->prepareStatement(
                "INSERT INTO Personal_Test_Qs(student_id, sub_id, quest_id) VALUES (?, ?, ?) "
                "ON DUPLICATE KEY UPDATE quest_id=quest_id"
            );
            pstmtQ->setInt(1, student_id);
            pstmtQ->setInt(2, sub_id);
            pstmtQ->setInt(3, stoi(questionsList[i].getId()));
            pstmtQ->execute();
            delete pstmtQ;

            // 3. Insert Responses
            if (answer[i] != -1) {
                sql::PreparedStatement* pstmt2 = con->prepareStatement(
                    "INSERT INTO Personal_Response(student_id, sub_id, quest_id, opt_id) "
                    "VALUES (?, ?, ?, ?) ON DUPLICATE KEY UPDATE opt_id = ?"
                );
                pstmt2->setInt(1, student_id);
                pstmt2->setInt(2, sub_id);
                pstmt2->setInt(3, stoi(questionsList[i].getId()));
                pstmt2->setInt(4, answer[i]);
                pstmt2->setInt(5, answer[i]);
                pstmt2->execute();
                delete pstmt2;
            }
        }

        // 4. Insert Score 
        sql::PreparedStatement* pstmt3 = con->prepareStatement(
            "INSERT INTO Personal_Result(student_id, sub_id, score) VALUES (?, ?, ?)"
        );
        pstmt3->setInt(1, student_id);
        pstmt3->setInt(2, sub_id);
        pstmt3->setInt(3, score);
        pstmt3->execute();

        delete pstmt3;
        delete con;
        cout << "\n[Success] Results saved to database successfully!\n";
    }
    catch (sql::SQLException& e) {
        cerr << "SQL Error saving attempt: " << e.what() << endl;
        delete con;
    }
}

// ---------------------------------------------------------
// ADMIN / VIEW DASHBOARD QUERIES
// ---------------------------------------------------------
void DatabaseStorage::addSubject() {
    string subjectName;
    cout << "\nEnter the new subject name: ";
    cin >> subjectName;

    sql::Connection* con = connectTodatabase();
    if (!con) return;

    try {
        sql::PreparedStatement* pstmt = con->prepareStatement(
            "INSERT INTO Subject (subject_name) VALUES (?)"
        );
        pstmt->setString(1, subjectName);
        pstmt->executeUpdate();

        cout << "[Success] Subject '" << subjectName << "' added to database.\n";

        delete pstmt;
        delete con;
    }
    catch (sql::SQLException& e) {
        cerr << "\n[Database Error] Could not add subject: " << e.what() << endl;
        delete con;
    }
}

void DatabaseStorage::addQuestion() {
    cout << "\n------------------------------------------------";
    cout << "\n[!] ADMIN NOTICE: Add Question Utility";
    cout << "\n------------------------------------------------";
    cout << "\nTo implement this fully, you will need to execute an INSERT into 'Questions', ";
    cout << "\nfetch the LAST_INSERT_ID(), and then run a loop to INSERT 4 options into 'Options'.\n";
}

void DatabaseStorage::viewQuestions() {
    sql::Connection* con = connectTodatabase();
    if (!con) return;

    try {
        sql::Statement* stmt = con->createStatement();
        sql::ResultSet* res = stmt->executeQuery(
            "SELECT q.quest_id, s.subject_name, q.difficulty, q.quest_txt "
            "FROM Questions q JOIN Subject s ON q.sub_id = s.sub_id"
        );

        cout << "\n=== ALL QUESTIONS IN DATABASE ===\n";
        while (res->next()) {
            cout << "ID: " << res->getString("quest_id")
                << " | Subject: " << res->getString("subject_name")
                << " | Diff: " << res->getInt("difficulty") << "\n"
                << "Q: " << res->getString("quest_txt") << "\n------------------------\n";
        }

        delete res;
        delete stmt;
        delete con;
    }
    catch (sql::SQLException& e) {
        cerr << "\n[Database Error] Could not view questions: " << e.what() << endl;
        delete con;
    }
}

void DatabaseStorage::viewResults(int student_id) {
    sql::Connection* con = connectTodatabase();
    if (!con) return;

    try {
        sql::PreparedStatement* pstmt = con->prepareStatement(
            "SELECT s.subject_name, pr.score, pr.attempt_time "
            "FROM Personal_Result pr "
            "JOIN Subject s ON pr.sub_id = s.sub_id "
            "WHERE pr.student_id = ? "
            "ORDER BY pr.attempt_time DESC" // Sorts newest to oldest!
        );
        pstmt->setInt(1, student_id);
        sql::ResultSet* res = pstmt->executeQuery();

        cout << "\n=== YOUR PAST ATTEMPTS ===\n";
        bool hasResults = false;

        while (res->next()) {
            hasResults = true;
            cout << "Subject: " << res->getString("subject_name")
                << " | Score: " << res->getInt("score")
                << " | Date: " << res->getString("attempt_time") << "\n";
        }

        if (!hasResults) {
            cout << "You have no past attempts recorded yet.\n";
        }

        delete res;
        delete pstmt;
        delete con;
    }
    catch (sql::SQLException& e) {
        cerr << "\n[Database Error] Could not fetch results: " << e.what() << endl;
        delete con;
    }
}