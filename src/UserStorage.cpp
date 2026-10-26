#include "UserStorage.h"
#include <fstream>
#include <sstream>
#include <openssl/md5.h>
#include <iomanip>
#include <algorithm>
#include <cctype>

bool UserStorage::loadFromFile(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) return false;

    std::string line;
    while (std::getline(file, line)) {
        if (line.empty()) continue;
        size_t delimiter = line.find(':');
        if (delimiter != std::string::npos) {
            std::string login = line.substr(0, delimiter);
            std::string password = line.substr(delimiter + 1);
            users[login] = password;
        }
    }
    return !users.empty();
}

std::string UserStorage::computeMd5(const std::string& salt, const std::string& password) {
    std::string data = salt + password;
    unsigned char digest[MD5_DIGEST_LENGTH];
    MD5(reinterpret_cast<const unsigned char*>(data.c_str()), data.size(), digest);

    std::ostringstream oss;
    for (int i = 0; i < MD5_DIGEST_LENGTH; ++i) {
        oss << std::hex << std::uppercase << std::setw(2) << std::setfill('0') << (int)digest[i];
    }
    return oss.str();
}

bool UserStorage::verifyCredentials(const std::string& login, const std::string& salt, const std::string& clientHash) {
    auto it = users.find(login);
    if (it == users.end()) return false;

    std::string serverHash = computeMd5(salt, it->second);
    return serverHash == clientHash;
}
