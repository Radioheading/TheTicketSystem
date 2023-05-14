#ifndef PARSE_PROGRAM_HPP_
#define PARSE_PROGRAM_HPP_
#include "train.hpp"
#include "user.hpp"
#include "../lib/tokenscanner.hpp"

class Program {
 private:
  UserSystem user_system;
  TrainSystem train_system;
  TokenScanner my_scanner;
 public:
  Program() : user_system("UserTree", "UserData"),
              train_system("TrainTree", "TrainData", "SeatTree", "SeatData", "StationTree", "StationData") {}

  void GetNew(const std::string &info) {
    my_scanner.set(info, ' ');
  }

  std::string parse() {
    std::string op, output;
    int time_stamp;
    my_scanner.GetChar(), time_stamp = my_scanner.NextInteger(), my_scanner.GetChar();
    output = '[' + std::to_string(time_stamp) + ']';

    op = my_scanner.NextToken();
    if (op == "login") {
      std::string op1, username, password;
      op1 = my_scanner.NextToken();
      if (op1 == "-u") {
        username = my_scanner.NextToken();
        my_scanner.NextToken();
        password = my_scanner.NextToken();
      } else {
        password = my_scanner.NextToken();
        my_scanner.NextToken();
        username = my_scanner.NextToken();
      }
      if (user_system.login(username, password)) {
        output += "0";
      } else {
        output += "-1";
      }
    } else if (op == "logout") {
      my_scanner.NextToken();
      std::string username = my_scanner.NextToken();
      my_scanner.NextToken();
      if (user_system.logout(username)) {
        output += "0";
      } else {
        output += "-1";
      }
    } else if (op == "query_profile") {
      my_scanner.NextToken();
      std::string op1, cur, username;
      op1 = my_scanner.NextToken();
      if (op1 == "-c") {
        cur = my_scanner.NextToken();
        my_scanner.NextToken();
        username = my_scanner.NextToken();
      } else {
        username = my_scanner.NextToken();
        my_scanner.NextToken();
        cur = my_scanner.NextToken();
      }
      output += user_system.query_profile(cur, username);
    } else if (op == "modify_profile") {
      std::string op1, cur, username, password, name, mailAddr;
      int privilege = -1;
      while (my_scanner.HasMoreToken()) {
        op1 = my_scanner.NextToken();
        if (op1 == "-c") cur = my_scanner.NextToken();
        if (op1 == "-u") username = my_scanner.NextToken();
        if (op1 == "-p") password = my_scanner.NextToken();
        if (op1 == "-n") name = my_scanner.NextToken();
        if (op1 == "-m") mailAddr = my_scanner.NextToken();
        if (op1 == "-g") privilege = my_scanner.NextInteger();
      }
      output += user_system.modify_profile(cur, username, password, name, mailAddr, privilege);
    } else if (op == "add_user") {
      std::string op1, cur, username, password, name, mailAddr;
      int privilege = -1;
      while (my_scanner.HasMoreToken()) {
        op1 = my_scanner.NextToken();
        if (op1 == "-c") cur = my_scanner.NextToken();
        if (op1 == "-u") username = my_scanner.NextToken();
        if (op1 == "-p") password = my_scanner.NextToken();
        if (op1 == "-n") name = my_scanner.NextToken();
        if (op1 == "-m") mailAddr = my_scanner.NextToken();
        if (op1 == "-g") privilege = my_scanner.NextInteger();
      }
      if (user_system.add_user(cur, username, password, name, mailAddr, privilege)) {
        output += "0";
      } else {
        output += "-1";
      }
    } else if (op == "add_train") {
      std::string op1, train_id, stations[101];
      int station_num, seat_num, prices[100], travel_time[100], stop_time[100];
      int start_h, start_m;
      char type;
      Date start_sale, end_sale;
      TokenScanner divide;
      while (my_scanner.HasMoreToken()) {
        op1 == my_scanner.NextToken();
        if (op1 == "-i") train_id = my_scanner.NextToken();
        if (op1 == "-n") station_num = my_scanner.NextInteger();
        if (op1 == "-m") seat_num = my_scanner.NextInteger();
        if (op1 == "-s") {
          divide.set(my_scanner.NextToken(), '|');
          int i = 1;
          while (divide.HasMoreToken()) {
            stations[i++] = divide.NextToken();
          }
        }
        if (op1 == "-p") {
          divide.set(my_scanner.NextToken(), '|');
          int i = 1;
          while (divide.HasMoreToken()) {
            prices[i++] = divide.NextInteger();
          }
        }
        if (op1 == "-x") {
          divide.set(my_scanner.NextToken(), ':');
          start_h = divide.NextInteger(), start_m = divide.NextInteger();
        }
        if (op1 == "-t") {
          divide.set(my_scanner.NextToken(), '|');
          int i = 1;
          while (divide.HasMoreToken()) {
            travel_time[i++] = divide.NextInteger();
          }
        }
        if (op1 == "-o") {
          divide.set(my_scanner.NextToken(), '|');
          int i = 1;
          std::string temp = divide.NextToken();
          if (temp == "-") {
            continue;
          } else {
            stop_time[i++] = std::stoi(temp);
          }
          while (divide.HasMoreToken()) {
            travel_time[i++] = divide.NextInteger();
          }
        }
        if (op1 == "-d") {
          divide.set(my_scanner.NextToken(), '|');
          TokenScanner start(divide.NextToken(), '-'), end(divide.NextToken(), '-');
          start_sale.change(2023, start.NextInteger(), start.NextInteger());
          end_sale.change(2023, end.NextInteger(), end.NextInteger());
        }
        if (op1 == "-y") type = my_scanner.GetChar();
      }
      if (train_system.add_train(train_id,
                                 station_num,
                                 seat_num,
                                 stations,
                                 prices,
                                 start_h * 60 + start_m,
                                 travel_time,
                                 stop_time,
                                 start_sale,
                                 end_sale,
                                 type)) {
        output += "0";
      } else {
        output += "-1";
      }
    } else if (op == "delete_train") {
      std::string op1 = my_scanner.NextToken(), train_id = my_scanner.NextToken();
      if (train_system.delete_train(train_id)) {
        output += "0";
      } else {
        output += "-1";
      }
    } else if (op == "release_train") {
      std::string op1 = my_scanner.NextToken(), train_id = my_scanner.NextToken();
      if (train_system.release_train(train_id)) {
        output += "0";
      } else {
        output += "-1";
      }
    } else if (op == "query_train") {
      std::string op1 = my_scanner.NextToken(), train_id;
      Date date;
      if (op1 == "-i") {
        train_id = my_scanner.NextToken();
        my_scanner.NextToken();
        TokenScanner divide(my_scanner.NextToken(), '-');
        date.change(2023, divide.NextInteger(), divide.NextInteger());
      } else {
        TokenScanner divide(my_scanner.NextToken(), '-');
        date.change(2023, divide.NextInteger(), divide.NextInteger());
        my_scanner.NextToken();
        train_id = my_scanner.NextToken();
      }
      output += train_system.query_train(date, train_id);
    } else if (op == "query_ticket") {
      std::string start, end, op1;
      bool cmp;
      Date date;
      while (my_scanner.HasMoreToken()) {
        op1 = my_scanner.NextToken();
        if (op1 == "-s") start = my_scanner.NextToken();
        if (op1 == "-t") start = my_scanner.NextToken();
        if (op1 == "-d") {
          TokenScanner divide(my_scanner.NextToken(), '-');
          date.change(2023, divide.NextInteger(), divide.NextInteger());
        }
        if (op1 == "-p") {
          if (my_scanner.NextToken() == "time") {
            cmp = true;
          } else {
            cmp = false;
          }
        }
      }
      output += train_system.query_ticket(start, end, date, cmp);
    } else if (op == "query_transfer") {
      std::string start, end, op1;
      bool cmp;
      Date date;
      while (my_scanner.HasMoreToken()) {
        op1 = my_scanner.NextToken();
        if (op1 == "-s") start = my_scanner.NextToken();
        if (op1 == "-t") start = my_scanner.NextToken();
        if (op1 == "-d") {
          TokenScanner divide(my_scanner.NextToken(), '-');
          date.change(2023, divide.NextInteger(), divide.NextInteger());
        }
        if (op1 == "-p") {
          if (my_scanner.NextToken() == "time") {
            cmp = true;
          } else {
            cmp = false;
          }
        }
      }
      output += train_system.query_transfer(start, end, date, cmp);
    } else if (op == "buy_ticket") {
      std::string op1, username, train_id, from, to;
      int num;
      Date date;
      bool wait;
      while (my_scanner.HasMoreToken()) {
        op1 = my_scanner.NextToken();
        if (op1 == "-u") username = my_scanner.NextToken();
        if (op1 == "-i") train_id = my_scanner.NextToken();
        if (op1 == "-d") {
          TokenScanner divide(my_scanner.NextToken(), '-');
          date.change(2023, my_scanner.NextInteger(), my_scanner.NextInteger());
        }
        if (op1 == "-n") num = my_scanner.NextInteger();
        if (op1 == "-f") from = my_scanner.NextToken();
        if (op1 == "-t") to = my_scanner.NextToken();
        if (op1 == "-q") wait = my_scanner.NextToken() == "true" ? true : false;
      }
      output += train_system.buy_ticket(username, train_id, date, from, to, wait, num, time_stamp);
    } else if (op == "query_order") {
      std::string username;
      my_scanner.NextToken(), username = my_scanner.NextToken();
      output += train_system.query_order(username);
    } else if (op == "refund_ticket") {
      std::string op1, username;
      int num;
      op1 = my_scanner.NextToken();
      if (op1 == "-u") {
        username = my_scanner.NextToken(), my_scanner.NextToken(), num = my_scanner.NextInteger();
      } else {
        num = my_scanner.NextInteger(), my_scanner.NextToken(), username = my_scanner.NextToken();
      }
      if (train_system.refund_ticket(username, num)) {
        output += "0";
      } else {
        output += "-1";
      }
    } else if (op == "clean") {
      user_system.clean();
      train_system.clean();
    } else if (op == "exit") {
      output += "bye";
    }
    return output;
  }
};
#endif

