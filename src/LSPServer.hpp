#pragma once
#include <string>

class LSPServer {
public:
    void run();
private:
    void handleRequest(const std::string& json);
    void sendResponse(int id, const std::string& result);
};
