#include "LSPServer.hpp"
#include <iostream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

void LSPServer::run() {
    std::string line;
    int contentLength = 0;

    while (true) {
        std::getline(std::cin, line);
        if (line.find("Content-Length:") == 0) {
            contentLength = std::stoi(line.substr(16));
        } else if (line == "\r" || line.empty()) {
            std::string body(contentLength, '\0');
            std::cin.read(&body[0], contentLength);

            // consume the trailing \r\n after the body
            std::cin.get(); 
            std::cin.get();

            handleRequest(body);
        }
    }
}

void LSPServer::handleRequest(const std::string& body) {
    auto request = json::parse(body);
    std::string method = request.value("method", "");

    if (method == "initialize") {
        int id = request.value("id", -1);
        if (id != -1) {
            json result = {
                {"capabilities", {
                    {"textDocumentSync", {
                        {"openClose", true},
                        {"change", 1}  // full text sync
                    }},
                    {"hoverProvider", true},
                    {"completionProvider", {
                        {"resolveProvider", false},
                        {"triggerCharacters", {"."}}
                    }}
                }}
            };
            sendResponse(id, result.dump());
        }
    }

    else if (method == "shutdown") {
        int id = request.value("id", -1);
        if (id != -1) sendResponse(id, "null");

    } else if (method == "exit") {
        std::exit(0);

    } else if (method == "textDocument/didOpen") {
        auto params = request["params"];
        std::string uri = params["textDocument"]["uri"];
        std::string text = params["textDocument"]["text"];
        documents[uri] = text;

        std::cerr << "[LSP] Opened file: " << uri << "\n";

    } else if (method == "textDocument/didChange") {
        auto params = request["params"];
        std::string uri = params["textDocument"]["uri"];
        auto contentChanges = params["contentChanges"];
        if (!contentChanges.empty()) {
            std::string newText = contentChanges[0]["text"];
            documents[uri] = newText;
        }
        std::cerr << "[LSP] Changed file: " << uri << "\n";

    } else if (method == "textDocument/hover") {
        auto params = request["params"];
        int id = request.value("id", -1);
        if (id == -1) return;

        std::string uri = params["textDocument"]["uri"];
        int line = params["position"]["line"];
        int character = params["position"]["character"];

        std::string hover = "Hover at line " + std::to_string(line) +
                            ", char " + std::to_string(character);

        if (documents.count(uri)) {
            const std::string& text = documents[uri];
            hover += "\nFile length: " + std::to_string(text.size());
        }

        json result = {
            {"contents", {
                {"kind", "markdown"},
                {"value", hover}
            }}
        };

        sendResponse(id, result.dump());
        std::cerr << "[LSP] Hover response sent for " << uri << "\n";
    }

    else {
        std::cerr << "[LSP] Unknown method: " << method << "\n";
    }
}

void LSPServer::sendResponse(int id, const std::string& resultStr) {
    json response = {
        {"jsonrpc", "2.0"},
        {"id", id},
        {"result", json::parse(resultStr)}
    };

    std::string out = response.dump();
    std::cout << "Content-Length: " << out.size() << "\r\n\r\n" << out << std::flush;
}
