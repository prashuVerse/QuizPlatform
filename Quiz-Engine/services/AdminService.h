#pragma once
#include "../persistence/DatabaseStorage.h"

class AdminService {
private:
    DatabaseStorage& storage;

public:
    AdminService(DatabaseStorage& storage);
    void run();
};