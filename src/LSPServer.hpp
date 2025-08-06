#pragma once
#include <string>
#include <unordered_map>

class LSPServer {
public:
    void run();
private:
    void handleRequest(const std::string& json);
    void sendResponse(int id, const std::string& result);

    std::unordered_map<std::string, std::string> documents; // uri → text
};
