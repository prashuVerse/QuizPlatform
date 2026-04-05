
#pragma once

#include "IStorage.h"
#include <vector>
#include "Attempt.h"
#include <mysql/jdbc.h>

class DatabaseStorage : public IStorage {
private:
	sql::Connection* connectToDatabase();

public:
	// Constructor
	DatabaseStorage();

	// Implementation of loadQuestions from IStorage
	std::vector<Question> loadQuestions() override;
	std::vector <Question> loadQuestions(string subject, int no_of_questions);

	void saveAttempt(const Attempt& attempt, int student_id, int sub_id, int score);
	int getSubjectId(string subjectName);
	// --- Authentication Functions ---
	int authenticateStudent(string email, string password);
	int registerStudent(string name, string email, string password, string mobile);

};
