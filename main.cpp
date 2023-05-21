/*
 * @todo   1、减少外存使用！SeatMap第二关键字为哈希值
 *         2、加快速度，部署modify
 */


#include <iostream>
#include "src/parse_program.hpp"

std::string operation;

int main() {
  srand(time(nullptr));
//  freopen("1.in", "r", stdin);
//  freopen("my_1.out", "w", stdout);
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
