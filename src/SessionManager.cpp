#include "SessionManager.h"
#include <unistd.h>
#include <iostream>
#include <arpa/inet.h>

SessionManager::SessionManager(int socketFd, Logger& log, UserStorage& storage, MathEngine& engine)
    : stream(socketFd), logger(log), userStorage(storage), mathEngine(engine) {}

void SessionManager::handleClient() {
    try {
        ProtocolHandler protocol(stream);
        
        AuthData authData = protocol.readAuthMessage();
        bool isAuthenticated = userStorage.verifyCredentials(authData.login, authData.salt, authData.hash);

        if (isAuthenticated) {
            protocol.writeStatus("OK");
            logger.log("INFO", "Client authenticated", "Login: " + authData.login);

            uint32_t vectorCount = protocol.readVectorCount();
            std::vector<int16_t> results;
            results.reserve(vectorCount);

            for (uint32_t i = 0; i < vectorCount; ++i) {
                std::vector<int16_t> vectorData = protocol.readVectorData();
                int16_t product = mathEngine.calculateProduct(vectorData);
                uint32_t recCount = htonl(1);
                int16_t netProduct = htons(product);
                
                protocol.writeExact(&recCount, sizeof(recCount));
                protocol.writeExact(&netProduct, sizeof(netProduct));
            }

            logger.log("INFO", "Session completed successfully", "Login: " + authData.login);
        } else {
            protocol.writeStatus("ERR");
            logger.log("WARNING", "Authentication failed", "Login: " + authData.login);
        }
    } catch (const std::exception& e) {
        logger.log("CRITICAL", "Session error", std::string("Exception: ") + e.what());
    }
}

void SessionManager::close() {
    if (stream >= 0) {
        ::close(stream);
        stream = -1;
    }
}
