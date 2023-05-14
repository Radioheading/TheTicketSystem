#include <iostream>
#include "src/parse_program.hpp"

using namespace std;

string operation;

int main() {
  ios::sync_with_stdio(false);
  cin.tie(nullptr), cout.tie(nullptr);
  Program TicketSystem;
  while (getline(cin, operation)) {
    TicketSystem.GetNew(operation);
    std::cout << TicketSystem.parse() << '\n';
  }
  return 0;
}
