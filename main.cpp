/*
 * @todo   1、调试basic6之后点的正确性
 *         3、调整bpt的modify
 *         4、调整bpt缓存的正确性
 */


#include <iostream>
#include "src/parse_program.hpp"

std::string operation;

int main() {
  srand(time(NULL));
//  freopen("3.in", "r", stdin);
//  freopen("my_3.out", "w", stdout);
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
