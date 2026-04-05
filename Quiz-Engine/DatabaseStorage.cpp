// This is DAL file responsible for talking with MYSQL
#include <iostream>
#include <vector>
#include <random>
#include <algorithm>
#include <string>
#include <sstream> 
#include "question.h"
#include "DatabaseStorage.h"

using namespace std;

#include <mysql/jdbc.h> // Mysql connector
#include <mysql/cppconn/prepared_statement.h>


sql::Connection* connectTodatabase() {
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

// RESTORED: Function to load ALL questions from the database
vector<Question> DatabaseStorage::loadQuestions() {
    vector<Question> questionsList;
    sql::Connection* con = connectTodatabase();

    if (con == nullptr) {
        cout << "Failed to connect to database: No connection Found" << endl;
        return questionsList;
    }

    try {
        sql::Statement* stmt = con->createStatement();
        // Fetch ALL questions across all subjects
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

            // Fetch Options for this specific question
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

            // Construct question with new parameters
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

            // Fetch Options for this specific question
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

    // Shuffle and pick requested amount
    random_device rd;
    mt19937 g(rd());
    shuffle(questionsList.begin(), questionsList.end(), g);

    vector<Question> SubjectWiseQuestionList;
    for (int i = 0; i < no_of_questions && i < questionsList.size(); i++) {
        SubjectWiseQuestionList.push_back(questionsList[i]);
    }
    return SubjectWiseQuestionList;
}

int DatabaseStorage::getSubjectId(string subjectName) {
    sql::Connection* con = connectTodatabase();
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

//Function of Authentication
int DatabaseStorage::authenticateStudent(string email, string password) {
    sql::Connection* con = connectToDatabase();
    if (!con) {
        cout << "Failed to connect to database:" << endl;
        return -1; 
    }

    int studentId = -1;
    try {
        //Join USer and Student table 
        sql :: PreparedStatement* pstmt = con->prepareStatement(
            "SELECT s.student_id FROM User u JOIN Student s ON u.user_id = s.user_id WHERE u.email = ? AND u.password = ?"
		);
        pstmt->setString(1, email);
        pstmt->setString(2, password);
        sql::ResultSet* res = pstmt->executeQuery();

        if (res->next()) {
            studentId = res->getInt("student_id"); // Success!
        }
        delete res;
        delete pstmt;
        delete con;
    }
    catch (sql::SQLException& e) {
        cerr << "Login Error: " << e.what() << endl;
        delete con;
    }
    return studentId;
}

int DatabaseStorage::registerStudent(string name, string email, string password, string mobile) {
    sql::Connection* con = connectToDatabase();
    if (!con) {
        cout << "Failed to connect to database:" << endl;
        return -1;
    }
    int newStudentId = -1;
    try {
        //Insert into USer Table
        sql::PreparedStatement* pstmt = con->prepareStatement(
            "INSERT INTO User (name, email, password) VALUES (?, ?, ?)"
        );
        pstmt->setString(1, name);
        pstmt->setString(2, email);
        pstmt->setString(3, password);
        pstmt->setString(4, mobile);
        pstmt->execute();
        delete pstmt;

        sql::Statement* stmt = con->createStatement();
        sql::ResultSet* res = stmt->executeQuery("SELECT LAST_INSERT_ID()");
        int userId = -1;
        if (res->next()) {
            userId = res->getInt(1);
        }
        delete res;

        if (userId != -1) {
            sql::PreparedStatement* pstmt2 = con->prepareStatement(
                "INSERT INTO Student (user_id) VALUES (?)"
            );
            pstmt2->setInt(1, userId);
            pstmt2->execute();
            delete pstmt2;

            // 4. Get the generated student_id to return
            sql::ResultSet* res2 = stmt->executeQuery("SELECT LAST_INSERT_ID()");
            if (res2->next()) {
                newStudentId = res2->getInt(1);
            }
            delete res2;
        }
        delete stmt;
        delete con;
    }
    catch (sql::SQLException& e) {
        cerr << "Registration Error: " << e.what() << endl;
        delete con;
    }
	return newStudentId;

}

// Function to save the quiz attempt
void DatabaseStorage::saveAttempt(const Attempt& attempt, int student_id, int sub_id, int score) {
    sql::Connection* con = connectTodatabase();
    if (!con) {
        cout << "Failed to connect to database:" << endl;
        return;
    }

    try {
        // 1. Insert Personal test
        sql::PreparedStatement* pstmt = con->prepareStatement(
            "INSERT INTO Personal_Test(student_id, sub_id) VALUES (?, ?) ON DUPLICATE KEY UPDATE student_id=student_id"
        );
        pstmt->setInt(1, student_id);
        pstmt->setInt(2, sub_id);
        pstmt->execute();
        delete pstmt;

        const vector<int>& answer = attempt.getAllAnswers(); 
        const vector<Question>& questionsList = attempt.getQuiz().getQuestion();

        for (int i = 0; i < questionsList.size(); i++) {
            
            // 2. FIX: Insert into Personal_Test_Qs (Tracks WHICH questions were served)
            sql::PreparedStatement* pstmtQ = con->prepareStatement(
                "INSERT INTO Personal_Test_Qs(student_id, sub_id, quest_id) VALUES (?, ?, ?) ON DUPLICATE KEY UPDATE quest_id=quest_id"
            );
            pstmtQ->setInt(1, student_id);
            pstmtQ->setInt(2, sub_id);
            pstmtQ->setInt(3, stoi(questionsList[i].getId()));
            pstmtQ->execute();
            delete pstmtQ;

            // 3. Insert Responses (Only if they actually answered it)
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
            "INSERT INTO Personal_Result(student_id, sub_id, score) VALUES (?, ?, ?) ON DUPLICATE KEY UPDATE score=?"
        );
        pstmt3->setInt(1, student_id);
        pstmt3->setInt(2, sub_id);
        pstmt3->setInt(3, score);
        pstmt3->setInt(4, score);
        pstmt3->execute();

        delete pstmt3;
        delete con;
        cout << "\n[Success] Results saved to database successfully!\n";
    }
    catch (sql::SQLException& e) {
        cerr << "SQL Error: " << e.what() << endl;
        delete con;
    }
}