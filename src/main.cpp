#include <iostream>
#include "LSPServer.hpp"

int main() {
  LSPServer* lsp = new  LSPServer();
  lsp->run();

  delete lsp;
  return 0;
}
