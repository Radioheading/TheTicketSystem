/*
 * @todo   1、想想换乘
 *         2、调试第二个点
 */


#include <iostream>
#include "src/parse_program.hpp"

using namespace std;

string operation;

int main() {
  freopen("1.in", "r", stdin);
  freopen("my_2.out", "w", stdout);
  ios::sync_with_stdio(false);
  cin.tie(nullptr), cout.tie(nullptr);
  Program TicketSystem;
  while (getline(cin, operation)) {
    try {
      TicketSystem.GetNew(operation);
      std::cout << TicketSystem.parse() << endl;
    } catch (...) {
      std::cout << "bye\n";
      return 0;
    }
  }
  return 0;
}
