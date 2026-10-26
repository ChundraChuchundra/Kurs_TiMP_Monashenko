#include "CServer.h"
#include "SessionManager.h"
#include <iostream>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

CServer::CServer() : serverSocket(-1) {}

CServer::~CServer() {
    if (serverSocket >= 0) {
        close(serverSocket);
    }
}

void CServer::showHelp() {
    std::cout << "Usage: vcalc_server [OPTIONS]\n"
              << "Options:\n"
              << "  -h, --help          Show this help message\n"
              << "  -d, --db <path>     Path to client database (default: /home/25pi208@ibst.psu/vcalc.conf)\n"
              << "  -l, --log <path>    Path to log file (default: /home/25pi208@ibst.psu/vcalc.log)\n"
              << "  -p, --port <port>   Server port (default: 33333)\n";
}

bool CServer::parseCommandLine(int argc, char* argv[]) {
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "-h" || arg == "--help") {
            showHelp();
            return false; // Завершаем работу после вывода справки
        } else if ((arg == "-d" || arg == "--db") && i + 1 < argc) {
            config.dbPath = argv[++i];
        } else if ((arg == "-l" || arg == "--log") && i + 1 < argc) {
            config.logPath = argv[++i];
        } else if ((arg == "-p" || arg == "--port") && i + 1 < argc) {
            config.port = std::stoi(argv[++i]);
        }
    }
    return true;
}

bool CServer::start(int argc, char* argv[]) {
    if (!parseCommandLine(argc, argv)) {
        return false;
    }

    if (!logger.init(config.logPath)) {
        std::cerr << "Не удалось инициализировать логер.\n";
        return false;
    }
    logger.log("INFO", "Server starting", "Port: " + std::to_string(config.port));

    if (!userStorage.loadFromFile(config.dbPath)) {
        logger.log("CRITICAL", "Failed to load user database", "Path: " + config.dbPath);
        std::cerr << "Не удалось загрузить БД из " << config.dbPath << "\n";
        return false;
    }
    logger.log("INFO", "User database loaded successfully", "Path: " + config.dbPath);

    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket < 0) {
        logger.log("CRITICAL", "Socket creation error");
        return false;
    }

    int opt = 1;
    setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(config.port);

    if (bind(serverSocket, (struct sockaddr*)&address, sizeof(address)) < 0) {
        logger.log("CRITICAL", "Bind failed", "Port: " + std::to_string(config.port));
        std::cerr << "Привязка не удалась\n";
        return false;
    }

    if (listen(serverSocket, 10) < 0) {
        logger.log("CRITICAL", "Listen failed");
        return false;
    }

    logger.log("INFO", "Server is listening", "Port: " + std::to_string(config.port));
    std::cout << "Сервер запущен на порту " << config.port << "...\n";

    while (true) {
        struct sockaddr_in clientAddr;
        socklen_t clientLen = sizeof(clientAddr);
        int clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &clientLen);
        
        if (clientSocket < 0) {
            logger.log("WARNING", "Accept failed");
            continue;
        }

        logger.log("INFO", "Client connected", "IP: " + std::string(inet_ntoa(clientAddr.sin_addr)));

        SessionManager session(clientSocket, logger, userStorage, mathEngine);
        session.handleClient();
        session.close();
    }

    return true;
}

int main(int argc, char* argv[]) {
    CServer server;
    if (!server.start(argc, argv)) {
        return 1;
    }
    return 0;
}
