/*
 * @todo   1、调试basic6之后点的正确性
 *         2、给用户类和火车类加上哈希函数
 *         3、调整bpt的modify
 *         4、调整bpt缓存的正确性
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
    try {
      TicketSystem.GetNew(operation);
      std::cout << TicketSystem.parse() << endl;
    } catch (...) {
      return 0;
    }
  }
  return 0;
}
