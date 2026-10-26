#pragma once
#include <string>
#include <vector>
#include <cstdint>

struct AuthData {
    std::string login;
    std::string salt;
    std::string hash;
};

class ProtocolHandler {
private:
    int stream; // Socket file descriptor

    // Вспомогательные методы для бинарного чтения/записи
    void readExact(void* buffer, size_t size);
    void writeExact(const void* buffer, size_t size);

public:
    explicit ProtocolHandler(int socketFd);

    AuthData readAuthMessage();
    uint32_t readVectorCount();
    std::vector<int16_t> readVectorData();
    
    void writeStatus(const std::string& status);
    void writeResults(const std::vector<int16_t>& results);
};