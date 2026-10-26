// UserStorage.h
#pragma once
#include <string>
#include <map>
#include <vector>

class UserStorage {
private:
    std::map<std::string, std::string> users;
    std::string computeMd5(const std::string& salt, const std::string& password);

public:
    bool loadFromFile(const std::string& path);
    bool verifyCredentials(const std::string& login, const std::string& salt, const std::string& clientHash);
};
