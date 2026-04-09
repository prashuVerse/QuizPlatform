#pragma once
#include "../persistence/DatabaseStorage.h"
#include <string>

class AuthService {
private:
    DatabaseStorage& storage; // Reference to your database connection

public:
    // Constructor injection
    AuthService(DatabaseStorage& storage);

    // Returns the student_id if successful, or -1 if failed
    int authenticate();
};