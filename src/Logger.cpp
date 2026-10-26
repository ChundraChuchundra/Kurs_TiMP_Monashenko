#include "Logger.h"
#include <iomanip>
#include <sstream>

Logger::~Logger() {
    if (logFile.is_open()) {
        logFile.close();
    }
}

bool Logger::init(const std::string& path) {
    logFilePath = path;
    logFile.open(logFilePath, std::ios::app);
    if (!logFile.is_open()) {
        std::cerr << "Не удалось открыть файл логов: : " << logFilePath << std::endl;
        return false;
    }
    return true;
}

void Logger::log(const std::string& level, const std::string& message, const std::string& params) {
    std::lock_guard<std::mutex> lock(mtx);
    if (!logFile.is_open()) return;

    auto now = std::time(nullptr);
    auto tm = *std::localtime(&now);

    std::ostringstream oss;
    oss << "[" << std::put_time(&tm, "%Y-%m-%d %H:%M:%S") << "] "
        << "[" << level << "] " << message;
    if (!params.empty()) {
        oss << " | Params: " << params;
    }
    oss << "\n";

    logFile << oss.str();
    logFile.flush();
}
