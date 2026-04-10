
#pragma once

#include "IStorage.h"
#include <vector>
#include "../core/Attempt.h"


#include <mysql/jdbc.h>

class DatabaseStorage : public IStorage {
private:
	sql::Connection* connectTodatabase();

public:
	// Constructor
	DatabaseStorage();

	// Implementation of loadQuestions from IStorage
	std::vector<Question> loadQuestions() override;
	std::vector <Question> loadQuestions(string subject, int no_of_questions);
	int registerStudent(std::string name, std::string email, std::string password);
	int loginStudent(std::string email, std::string password);
	void saveAttempt(const Attempt& attempt, int student_id, int sub_id, int score);
	int getSubjectId(string subjectName);
	void addSubject();
	void addQuestion();
	void viewQuestions();
	void viewResults(int student_id);
	void showAvailableSubjects();

};
