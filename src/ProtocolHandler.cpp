#include "ProtocolHandler.h"
#include <unistd.h>
#include <stdexcept>
#include <sstream>
#include <arpa/inet.h>

ProtocolHandler::ProtocolHandler(int socketFd) : stream(socketFd) {}

void ProtocolHandler::readExact(void* buffer, size_t size) {
    size_t totalRead = 0;
    while (totalRead < size) {
        ssize_t readBytes = read(stream, static_cast<char*>(buffer) + totalRead, size - totalRead);
        if (readBytes <= 0) throw std::runtime_error("Connection closed or read error");
        totalRead += readBytes;
    }
}

void ProtocolHandler::writeExact(const void* buffer, size_t size) {
    size_t totalWritten = 0;
    while (totalWritten < size) {
        ssize_t writtenBytes = write(stream, static_cast<const char*>(buffer) + totalWritten, size - totalWritten);
        if (writtenBytes <= 0) throw std::runtime_error("Connection closed or write error");
        totalWritten += writtenBytes;
    }
}

AuthData ProtocolHandler::readAuthMessage() {
    std::string msg;
    char c;
    while (true) {
        if (read(stream, &c, 1) <= 0) throw std::runtime_error("Connection closed during auth");
        if (c == '\n') break;
        msg += c;
    }

    AuthData data;
    std::istringstream iss(msg);
    iss >> data.login >> data.salt >> data.hash;
    data.login = msg.substr(0, 4);
    data.salt = msg.substr(4, 16);
    data.hash = msg.substr(20, 32);
    return data;
}

uint32_t ProtocolHandler::readVectorCount() {
    uint32_t count;
    readExact(&count, sizeof(count));
    return ntohl(count); // Перевод из сетевого порядка байт в хостовый
}

std::vector<int16_t> ProtocolHandler::readVectorData() {
    uint32_t size;
    readExact(&size, sizeof(size));
    size = ntohl(size);

    std::vector<int16_t> data(size);
    if (size > 0) {
        readExact(data.data(), size * sizeof(int16_t));
        for (uint32_t i = 0; i < size; ++i) {
        data[i] = ntohs(data[i]);
        }
    }
    return data;
}

void ProtocolHandler::writeStatus(const std::string& status) {
    std::string msg = status + "\n";
    writeExact(msg.c_str(), msg.size());
}

void ProtocolHandler::writeResults(const std::vector<int16_t>& results) {
    uint32_t count = htonl(static_cast<uint32_t>(results.size()));
    writeExact(&count, sizeof(count));
    
    if (!results.empty()) {
        writeExact(results.data(), results.size() * sizeof(int16_t));
    }
}
