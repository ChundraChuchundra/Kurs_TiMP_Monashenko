#pragma once
#include "Logger.h"
#include "UserStorage.h"
#include "MathEngine.h"
#include <string>

struct ServerConfig {
    std::string dbPath = "/home/25pi208@ibst.psu/vcalc.conf";
    std::string logPath = "/home/25pi208@ibst.psu/vcalc.log";
    int port = 33333;
};

class CServer {
private:
    ServerConfig config;
    Logger logger;
    UserStorage userStorage;
    MathEngine mathEngine;
    int serverSocket;

    void showHelp();
    bool parseCommandLine(int argc, char* argv[]);

public:
    CServer();
    ~CServer();
    bool start(int argc, char* argv[]);
};