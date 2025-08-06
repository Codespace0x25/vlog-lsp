#include "LSPServer.hpp"
#include <iostream>
#include <sstream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

void LSPServer::run() {
    std::string line;
    int contentLength = 0;

    while (std::getline(std::cin, line)) {
        if (line.find("Content-Length:") == 0) {
            contentLength = std::stoi(line.substr(16));
        } else if (line == "\r" || line.empty()) {
            std::string body(contentLength, '\0');
            std::cin.read(&body[0], contentLength);
            handleRequest(body);
        }
    }
}

void LSPServer::handleRequest(const std::string& body) {
    auto request = json::parse(body);
    std::string method = request["method"];

    if (method == "initialize") {
        json result = {
            { "capabilities", {
                { "textDocumentSync", 1 },
                { "hoverProvider", true },
                { "completionProvider", {
                    { "resolveProvider", false },
                    { "triggerCharacters", {"."} }
                }}
            }}
        };
        sendResponse(request["id"], result.dump());
    } else if (method == "shutdown") {
        sendResponse(request["id"], "null");
    } else if (method == "exit") {
        std::exit(0);
    }
}

void LSPServer::sendResponse(int id, const std::string& result) {
    json response = {
        { "jsonrpc", "2.0" },
        { "id", id },
        { "result", json::parse(result) }
    };
    std::string out = response.dump();
    std::cout << "Content-Length: " << out.size() << "\r\n\r\n" << out << std::flush;
}
