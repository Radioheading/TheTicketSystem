/*
 * @todo 1、加快速度，部署modify
 *       2、丰富文件结构
 */


#include <iostream>
#include "src/parse_program.hpp"

std::string operation;

int main() {
  srand(time(nullptr));
  std::ios::sync_with_stdio(false);
  std::cin.tie(nullptr), std::cout.tie(nullptr);
  Program TicketSystem;
  while (getline(std::cin, operation)) {
    try {
      TicketSystem.GetNew(operation);
      std::cout << TicketSystem.parse() << '\n';
    } catch (...) {
      return 0;
    }
  }
  return 0;
}
