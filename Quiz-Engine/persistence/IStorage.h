#pragma once

#include <vector>
#include "../core/Question.h"

class IStorage {
public:
	// Load all questions from the storage system
	virtual std::vector<Question> loadQuestions() = 0;

	// Virtual destructor for proper cleanup
	virtual ~IStorage() {}


};
