#pragma once
#include <string>

using namespace std;
//only setting the user in this header file , auth no included in this header. auth to be implemented as a service not core
class User {
private:
    string userName;
public:
    User(string userName);
    string getUserName() const;

};