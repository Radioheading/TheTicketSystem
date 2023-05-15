/*
 * @todo   1、想想换乘
 *         2、可以开始调试第一个点
 */


#include <iostream>
#include "src/parse_program.hpp"

using namespace std;

string operation;

int main() {
  freopen("1.in", "r", stdin);
  freopen("my_1.out", "w", stdout);
  ios::sync_with_stdio(false);
  cin.tie(nullptr), cout.tie(nullptr);
  Program TicketSystem;
  while (getline(cin, operation)) {
    TicketSystem.GetNew(operation);
    std::cout << TicketSystem.parse() << '\n';
  }
  return 0;
}
