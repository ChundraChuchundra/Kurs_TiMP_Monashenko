#pragma once
#include <string>
#include <fstream>
#include <mutex>
#include <ctime>
#include <iostream>

class Logger {
private:
    std::string logFilePath;
    std::ofstream logFile;
    std::mutex mtx;

public:
    Logger() = default;
    ~Logger();

    bool init(const std::string& path);
    void log(const std::string& level, const std::string& message, const std::string& params = "");
};