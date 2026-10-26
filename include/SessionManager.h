// SessionManager.h
#pragma once
#include "ProtocolHandler.h"
#include "UserStorage.h"
#include "MathEngine.h"
#include "Logger.h"

class SessionManager {
private:
    int stream;
    Logger& logger;
    UserStorage& userStorage;
    MathEngine& mathEngine;

public:
    SessionManager(int socketFd, Logger& log, UserStorage& storage, MathEngine& engine);
    void handleClient();
    void close();
};