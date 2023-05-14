/*
 * @todo : 1、修改B+树，前面的两项为第一、第二关键字，然后更改索引的结构
 *         2、梳理每个结构体的内容，重新修改B+树的逻辑
 *         3、写好vector的排序函数
 *         4、（如果还有时间的话），写完剩下的函数
 */


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
