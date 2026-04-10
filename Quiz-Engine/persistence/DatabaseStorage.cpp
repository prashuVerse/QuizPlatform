// This is DAL file responsible for talking with MYSQL
#include <iostream>  //input output
#include <vector>    //dynmic array
#include <random>    //ranfom number generator
#include <algorithm>  //function like shuffle
#include <string>     //string handling
#include <sstream> 
#include "../core/question.h"
#include "DatabaseStorage.h"

using namespace std;

#include <mysql/jdbc.h> // Mysql connector
#include <jdbc/cppconn/prepared_statement.h>

DatabaseStorage::DatabaseStorage() {}  //empty constructor nothing happen when object is created

sql::Connection* DatabaseStorage::connectTodatabase() {  //Function returns pointer to MySQL connection
    try {
        sql::Driver* driver = get_driver_instance();  //Get MySQL driver instance
        sql::Connection* con = driver->connect("tcp://127.0.0.1:3306", "root", "Rashmi@123");//Connect to mySQl  localHost, port, username, password
        con->setSchema("quiz_engine"); //Selcect the database to use
        return con;  //return connection
    }
    //if error occur then return null pointer
    catch (sql::SQLException& e) {
        std::cerr << "Database Connection Error: " << e.what() << std::endl;
        return nullptr;
    }
}

// Function to load ALL questions from the database
vector<Question> DatabaseStorage::loadQuestions() {
    vector<Question> questionsList;  //store all question
    sql::Connection* con = connectTodatabase();  //get db connection

    if (con == nullptr) {
        cout << "Failed to connect to database: No connection Found" << endl;
        return questionsList;
    }

    try {
        sql::Statement* stmt = con->createStatement();  //create sql statement 

        //fetch the q_id,subject, difficulty,text from question and subject table and store in result
        sql::ResultSet* res = stmt->executeQuery(
            "SELECT q.quest_id, s.subject_name, q.difficulty, q.quest_txt "
            "FROM Questions q JOIN Subject s ON q.sub_id = s.sub_id"
        );

        while (res->next()) { //iterate each row 

            //extract the data from each row 
            string qid = res->getString("quest_id");
            string qSubject = res->getString("subject_name");
            int qDifficulty = res->getInt("difficulty");
            string qText = res->getString("quest_txt");

            vector<string> optionList;
            vector<int> optionIds;
            int cOptId = -1;

			sql::PreparedStatement* optStmt = con->prepareStatement(          //select option query to fetch options for each question
                "SELECT opt_id, option_txt, is_correct FROM Options WHERE quest_id = ?"
            );

			optStmt->setInt(1, stoi(qid)); //the query return the question id in string format but we need to pass it as int to query so we convert it using stoi function
			sql::ResultSet* optRes = optStmt->executeQuery();  //execute the query and store the result in optRes

			while (optRes->next()) { //iterate each option for the question
				//extract the option id, text and correctness from each row

                int optId = optRes->getInt("opt_id"); 
                optionList.push_back(optRes->getString("option_txt"));
                optionIds.push_back(optId);

				if (optRes->getBoolean("is_correct")) {   //identify the correct option and store its id in cOptId
                    cOptId = optId;
                }
            }
            delete optRes;
            delete optStmt;

			Question newQuestion(qid, qSubject, qDifficulty, qText, optionList, optionIds, cOptId); //create a new question object using the extracted data and add it to the questions list
            questionsList.push_back(newQuestion); //store the question object in the vector of quesiton
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
vector<Question> DatabaseStorage::loadQuestions(string subject, int no_of_questions) { //function take two values subject and no_of_quesiton 
	vector<Question> questionsList; //create empty bucket to store the question
    sql::Connection* con = connectTodatabase();  //create a sql connection to database

    if (con == nullptr) {
        cout << "Failed to connect to database: No connection Found" << endl;
        return questionsList;
    }

    try {

        //prepare statement
        //Unlike createStatement(), this creates a secure template with a ? blank spot. 
        // This is crucial because the subject is coming from user input, and this protects you from SQL injection hackers.
        sql::PreparedStatement* pstmt = con->prepareStatement(    
            "SELECT q.quest_id, s.subject_name, q.difficulty, q.quest_txt "
            "FROM Questions q "
            "JOIN Subject s ON q.sub_id = s.sub_id "
            "WHERE s.subject_name = ?"
        );
		pstmt->setString(1, subject);  //safely lock the word into the first blank spot of the query and execute it
        sql::ResultSet* res = pstmt->executeQuery();  //execute the querry and store the result in res 

		while (res->next()) { // go through database rows one by one until we have all the questions for that subject
            string qid = res->getString("quest_id");
            string qSubject = res->getString("subject_name");
            int qDifficulty = res->getInt("difficulty");
            string qText = res->getString("quest_txt");

            vector<string> optionList;
            vector<int> optionIds;
            int cOptId = -1;

            //Creates a second secure query to fetch only the options that belong to the current quest_id.
            sql::PreparedStatement* optStmt = con->prepareStatement(
                "SELECT opt_id, option_txt, is_correct FROM Options WHERE quest_id = ?"
            );
            optStmt->setInt(1, stoi(qid)); // stoi Translates the text ID (like "15") into the math integer 15 so the database can read it.
			sql::ResultSet* optRes = optStmt->executeQuery(); //execute the query and store the result in optRes

			while (optRes->next()) { //iterate through the options for the current question and extract the option id, text and correctness from each row
                int optId = optRes->getInt("opt_id");
                optionList.push_back(optRes->getString("option_txt"));
                optionIds.push_back(optId);

                if (optRes->getBoolean("is_correct")) {
                    cOptId = optId;
                }
            }
            delete optRes;
            delete optStmt;

			Question newQuestion(qid, qSubject, qDifficulty, qText, optionList, optionIds, cOptId); //create a new question object using the extracted data and add it to the questions list
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

	//now quesiton list contain every subject wise question but we only want to give the number of question
    // that student ask for so we shuffle the question list and then pick the number of question from the top of the list and return it to quiz engine
    random_device rd;
    mt19937 g(rd());
    shuffle(questionsList.begin(), questionsList.end(), g);

	vector<Question> SubjectWiseQuestionList;  //new bucket to store the number of question that student ask for
    for (int i = 0; i < no_of_questions && i < questionsList.size(); i++) {
        SubjectWiseQuestionList.push_back(questionsList[i]);
    }
    return SubjectWiseQuestionList;
}

int DatabaseStorage::getSubjectId(string subjectName)  {
    // Note: To fix the 'const' error from the header, we must cast away constness 
    // just for the connection, or remove 'const' from the header. 
    // Assuming the header has 'const', we'll use a const_cast for the DB connection.
    //const (meaning read-only, it won't change the DatabaseStorage object). 
    // However, opening a database connection sometimes alters internal memory. 
    // const_cast tells the compiler, "I know I promised not to touch anything, but I need you to look the other way for one second so I can open this connection."
    sql::Connection* con = const_cast<DatabaseStorage*>(this)->connectTodatabase();
    if (!con) return -1;

    int subId = -1;
    try {
		// Prepare a secure statement to fetch the subject ID based on the subject name
        sql::PreparedStatement* pstmt = con->prepareStatement(
            "SELECT sub_id FROM Subject WHERE subject_name = ?"
        );
		pstmt->setString(1, subjectName); // Safely set the subject name into the query
		sql::ResultSet* res = pstmt->executeQuery(); // Execute the query and store the result

		if (res->next()) { // If a matching subject is found, extract the sub_id
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
int DatabaseStorage::registerStudent(string name, string email, string password) { //function take three values name, email and password and return the student id if registration is successful otherwise return -1
	sql::Connection* con = connectTodatabase(); //create a sql connection to database
    if (!con) {
        cout << "Failed to connect to database." << endl;
        return -1;
    }

    try {
        // 1. Insert into USERS table (Notice email_id and 'Student' role)
		// We use a prepared statement to safely insert user data and prevent SQL injection
        sql::PreparedStatement* pstmtUser = con->prepareStatement(
            "INSERT INTO USERS(name, email_id, password, role) VALUES (?, ?, ?, 'Student')"
        );
		pstmtUser->setString(1, name); // Safely set the name into the query
		pstmtUser->setString(2, email);// Safely set the email into the query
		pstmtUser->setString(3, password); // safely set the password into the query
		pstmtUser->executeUpdate(); // Execute the insert statement to add the new user to the USERS table
		delete pstmtUser; // Clean up the prepared statement to free resources

        // 2. Fetch the generated user_id
		sql::Statement* stmt = con->createStatement(); // Create a new statement to execute a simple query
		sql::ResultSet* res = stmt->executeQuery("SELECT LAST_INSERT_ID() AS user_id"); // Execute a query to get the last auto-incremented ID generated by the previous insert (the new user's ID)

		int userId = -1; // Initialize userId to -1 to indicate an error state if we fail to retrieve it
        if (res->next()) { 
			userId = res->getInt("user_id"); //reaches the row containing the generated user_id and extracts it into the userId variable
        }
        delete res;
        delete stmt;

        if (userId == -1) {
            delete con;
            return -1;
        }

        // 3. Insert into Student table
		// Now that we have the user_id, we can insert a new record into the Student table to link it to the USERS table
        sql::PreparedStatement* pstmtStudent = con->prepareStatement(
            "INSERT INTO Student(user_id) VALUES (?)"
        );
		pstmtStudent->setInt(1, userId); // Safely set the user_id into the query to link the Student record to the correct user
		pstmtStudent->executeUpdate(); // Execute the insert statement to add the new student record to the Student table
        delete pstmtStudent;

        // 4. Fetch the generated student_id
		// After inserting into the Student table, we need to retrieve the generated student_id to return it to the caller
        stmt = con->createStatement();
        res = stmt->executeQuery("SELECT LAST_INSERT_ID() AS student_id");

		int studentId = -1; // Initialize studentId to -1 to indicate an error state if we fail to retrieve it
        if (res->next()) {
			studentId = res->getInt("student_id"); // Reaches the row containing the generated student_id and extracts it into the studentId variable
        }
        delete res;
        delete stmt;
        delete con;

		return studentId; // Return the new student's ID to indicate successful registration

    }
    catch (sql::SQLException& e) {
        cerr << "\n[Database Error] Registration failed: " << e.what() << endl;
        delete con;
        return -1;
    }
}

int DatabaseStorage::loginStudent(string email, string password) {
	sql::Connection* con = connectTodatabase(); //create a sql connection to database
    if (!con) {
        cout << "Failed to connect to database." << endl;
        return -1;
    }

    try {
		// Prepare a secure statement to fetch the student_id based on the provided email and password
        sql::PreparedStatement* pstmt = con->prepareStatement(
            "SELECT s.student_id FROM Student s "
            "JOIN USERS u ON s.user_id = u.user_id "
            "WHERE u.email_id = ? AND u.password = ?"
        );
		pstmt->setString(1, email); // Safely set the email into the query to find the user
		pstmt->setString(2, password); // Safely set the password into the query to verify credentials

		sql::ResultSet* res = pstmt->executeQuery(); // Execute the query and store the result, which should contain the student_id if the credentials are correct

		int studentId = -1; // Initialize studentId to -1 to indicate an error state if login fails (e.g., no matching record found)
        if (res->next()) {
			studentId = res->getInt("student_id"); // If a matching record is found, extract the student_id from the result set and store it in the studentId variable to return to the caller
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
    cout << "\n=== ADD NEW QUESTION ===" << endl;

    //  Get Subject
    string subjectName;
    cout << "Enter Subject Name (e.g., DBMS): ";
    cin >> subjectName; 

    // Look up the subject ID using your existing function!
    int sub_id = getSubjectId(subjectName);
    if (sub_id == -1) {
        cout << "\n[!] Subject not found! Please add the subject first using the Admin Menu.\n";
        return;
    }

    //  Get Difficulty
    int difficulty;
    cout << "Enter Difficulty (1=Easy, 2=Medium, 3=Hard): ";
    cin >> difficulty;

   //When switching from 'cin >>' to 'getline', you must clear the input buffer!
    cin.ignore();

    // Get Question Text
    string questText; 
    cout << "Enter Question Text: ";
    getline(cin, questText); // getline allows the admin to type spaces in their question! 

    // Get the 4 Options
    vector<string> options(4);
    for (int i = 0; i < 4; i++) {
        cout << "Enter Option " << (i + 1) << ": ";
        getline(cin, options[i]);
    }

    // Get Correct Option
    int correctIndex;
    cout << "Which option is correct? (1, 2, 3, or 4): ";
    cin >> correctIndex;

    // --- DATABASE INSERTION ---
    sql::Connection* con = connectTodatabase();
    if (!con) return;

    try {
        // Insert the main question
        sql::PreparedStatement* pstmtQ = con->prepareStatement(
            "INSERT INTO Questions (sub_id, difficulty, quest_txt) VALUES (?, ?, ?)"
        );
        pstmtQ->setInt(1, sub_id);
        pstmtQ->setInt(2, difficulty);
        pstmtQ->setString(3, questText);
        pstmtQ->executeUpdate();
        delete pstmtQ;

        // B. Fetch the ID that MySQL just auto-generated for this question
        sql::Statement* stmt = con->createStatement();
        sql::ResultSet* res = stmt->executeQuery("SELECT LAST_INSERT_ID() AS quest_id");

        int new_quest_id = -1;
        if (res->next()) {
            new_quest_id = res->getInt("quest_id");
        }
        delete res;
        delete stmt;

        if (new_quest_id == -1) {
            cout << "\n[!] Failed to retrieve the new Question ID." << endl;
            delete con;
            return;
        }

        // Insert the 4 Options linked to this new question
        sql::PreparedStatement* pstmtOpt = con->prepareStatement(
            "INSERT INTO Options (quest_id, option_txt, is_correct) VALUES (?, ?, ?)"
        );

        for (int i = 0; i < 4; i++) {
            pstmtOpt->setInt(1, new_quest_id);
            pstmtOpt->setString(2, options[i]);

            // If i+1 matches what the admin typed, save 'true' (1), otherwise 'false' (0)
            pstmtOpt->setBoolean(3, (i + 1 == correctIndex));

            pstmtOpt->executeUpdate();
        }

        delete pstmtOpt;
        delete con;

        cout << "\n[Success] Question added to database successfully!" << endl;

    }
    catch (sql::SQLException& e) {
        cerr << "\n[Database Error] Could not add question: " << e.what() << endl;
        delete con;
    }
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

void DatabaseStorage::showAvailableSubjects() {
    sql::Connection* con = connectTodatabase();
    if (!con) return;

    try {
        sql::Statement* stmt = con->createStatement();
        sql::ResultSet* res = stmt->executeQuery("SELECT subject_name FROM Subject");

        cout << "\n=== AVAILABLE SUBJECTS ===" << endl;
        bool found = false;

        while (res->next()) {
            found = true;
            cout << "  > " << res->getString("subject_name") << endl;
        }

        if (!found) {
            cout << "  (No subjects available yet)" << endl;
        }
        cout << "==========================" << endl;

        delete res;
        delete stmt;
        delete con;
    }
    catch (sql::SQLException& e) {
        cerr << "\n[Database Error] Could not fetch subjects: " << e.what() << endl;
        delete con;
    }
}