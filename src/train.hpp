#ifndef TRAIN_HPP_
#define TRAIN_HPP_

#include <utility>

#include "../lib/utils.hpp"
#include "../lib/bpt.hpp"
#include "../lib/map.hpp"
#include "../lib/mutibpt.hpp"
#include "../lib/exceptions.hpp"
#include "../lib/vector.hpp"

const int max_info = 101;

using namespace Lee;

struct Train { // the basic information of a train
  Date start_sale{}, end_sale{};
  int stationNum{}, seatNum{}, startTime{};
  bool release_state = false;
  char type{};
  my_string<20> trainID{};
  my_string<40> stations[max_info]{};
  int prices_sum[max_info]{};
  int leave_time[max_info]{}, arrive_time[max_info]{0};

  Train() = default;

  Train(const Train &other) {
    release_state = other.release_state, trainID = other.trainID;
    stationNum = other.stationNum, seatNum = other.seatNum, startTime = other.startTime;
    start_sale = other.start_sale, end_sale = other.end_sale, type = other.type;
    for (int i = 1; i <= stationNum; ++i) {
      stations[i] = other.stations[i], prices_sum[i] = other.prices_sum[i];
      leave_time[i] = other.leave_time[i], arrive_time[i] = other.arrive_time[i];
    }
  }

  Train &operator=(const Train &other) {
    if (&other == this) return *this;
    release_state = other.release_state;
    stationNum = other.stationNum, seatNum = other.seatNum, startTime = other.startTime;
    trainID = other.trainID, type = other.type;
    start_sale = other.start_sale, end_sale = other.end_sale;
    for (int i = 1; i <= stationNum; ++i) {
      stations[i] = other.stations[i], prices_sum[i] = other.prices_sum[i];
      leave_time[i] = other.leave_time[i], arrive_time[i] = other.arrive_time[i];
    }
    return *this;
  }
};

struct train_seat {
  int station_num{0}, seat_num{-1};
  int seat[max_info - 1]{};

  train_seat(int _station_num, int _seat_num)
      : station_num(_station_num), seat_num(_seat_num) {
    for (int i = 0; i < station_num; ++i) seat[i] = seat_num;
  }

  train_seat() : station_num(0), seat_num(0) {};
};

int available(const train_seat &seat, int l, int r) {
  // query the maximum ticket storage between the lth station and the rth
  int ans;
  for (int i = l - 1; i < r; ++i) {
    if (i == l - 1) {
      ans = seat.seat[l - 1];
    } else {
      ans = std::min(seat.seat[i], ans);
    }
  }
  return ans;
}

void satisfy_order(train_seat &seat, int l, int r, int num) {
  // used to buy/refund tickets between rank l and rank r
  for (int i = l - 1; i < r; ++i) {
    seat.seat[i] -= num;
  }
}

struct id_date {
  size_t id{};
  Date date; // the day when this train starts

  friend std::ostream &operator<<(std::ostream &out, const id_date &todo) {
    out << todo.id << ' ' << todo.date;
    return out;
  }

  id_date() = default;

  id_date(size_t _id, const Date &_date) : id(_id), date(_date) {}

  friend bool operator<(const id_date &cmp_1, const id_date &cmp_2) {
    return cmp_1.id < cmp_2.id || cmp_1.id == cmp_2.id && cmp_1.date < cmp_2.date;
  }

  friend bool operator==(const id_date &cmp_1, const id_date &cmp_2) {
    return cmp_1.id == cmp_2.id && cmp_1.date == cmp_2.date;
  }
};

struct station_train { // the basic information of a train passing a given station
  size_t id_key{};
  int leave{}, arrive{}, prices_sum{}, rank{};
  my_string<20> train_id;
  station_train() = default;
  explicit station_train(const std::string &_train_id) : train_id(_train_id) {}
  station_train(int _leave, int _arrive, int _sum, const my_string<20> &id, int _rank, int _start_time)
      : leave(_leave), arrive(_arrive), prices_sum(_sum), rank(_rank),
        train_id(id), id_key(MyHash(id)) {};
};

struct ticket_info { // the basic information of a ticket(query_ticket, query_transfer)
  int price{}, seat{}, duration{};
  my_string<20> train_id;
  Time depart, arrive;

  ticket_info() = default;

  ticket_info(const my_string<20> &_train_id, Time _depart, Time _arrive,
              int _price, int _seat, int _duration) : train_id(_train_id), depart(_depart),
                                                      arrive(_arrive),
                                                      price(_price),
                                                      seat(_seat),
                                                      duration(_duration) {}

  friend bool operator!=(const ticket_info &cmp_1, const ticket_info &cmp_2) {
    return !(cmp_1.train_id == cmp_2.train_id) || cmp_1.depart != cmp_2.depart;
  }
  static bool TimeFirst(const ticket_info &cmp_1, const ticket_info &cmp_2) {
    return cmp_1.duration < cmp_2.duration || cmp_1.duration == cmp_2.duration && cmp_1.train_id < cmp_2.train_id;
  }

  static bool CostFirst(const ticket_info &cmp_1, const ticket_info &cmp_2) {
    return cmp_1.price < cmp_2.price || cmp_1.price == cmp_2.price && cmp_1.train_id < cmp_2.train_id;
  }
};

static bool (*TimeCmp)(const ticket_info &, const ticket_info &) {
    ticket_info::TimeFirst
};

static bool (*CostCmp)(const ticket_info &, const ticket_info &) {
    ticket_info::CostFirst
};

enum TicketState { Success, Pending, Refunded };

struct order {
  TicketState state = Success;
  int price{}, num{}, time_stamp{}, rank_s{}, rank_e{};
  my_string<20> train_id;
  my_string<40> from, to;
  Time departure, arrival;
  Date first_leave;

  explicit order(int _time = 2147483647) : time_stamp(_time) {};

  order(TicketState _state, int _price, int _num, int _time_stamp, int _rank_s, int _rank_e,
        const std::string &_train_id, const std::string &_from, const std::string &_to,
        Time _departure, Time _arrival, Date _first_leave) : state(_state),
                                                             price(_price),
                                                             num(_num),
                                                             time_stamp(_time_stamp),
                                                             train_id(_train_id),
                                                             from(_from),
                                                             to(_to),
                                                             rank_s(_rank_s),
                                                             rank_e(_rank_e),
                                                             departure(_departure),
                                                             arrival(_arrival),
                                                             first_leave(_first_leave) {}
  explicit operator std::string() const {
    std::string out = "[";
    if (state == Success) out += "success] ";
    if (state == Pending) out += "pending] ";
    if (state == Refunded) out += "refunded] ";
    out += (std::string) train_id + ' ' + (std::string) from + ' ' + (std::string) departure;
    out += " -> " + (std::string) to + ' ' + (std::string) arrival + ' ' + std::to_string(price) + ' '
        + std::to_string(num);
    return out;
  }
};

struct pending {
  int num{}, time_stamp{}, start_rank{}, end_rank{};
  my_string<20> username;

  explicit pending(int _time_stamp = 0) : time_stamp(_time_stamp) {};
  pending(int _num, int _time_stamp, int _start_rank,
          int _end_rank, const std::string &_username) : num(_num), time_stamp(_time_stamp),
                                                         start_rank(_start_rank), end_rank(_end_rank),
                                                         username(_username) {}

  friend bool operator<(const pending &cmp_1, const pending &cmp_2) {
    return cmp_1.time_stamp < cmp_2.time_stamp;
  }

  friend bool operator==(const pending &cmp_1, const pending &cmp_2) {
    return cmp_1.time_stamp == cmp_2.time_stamp;
  }
};

class TrainSystem {
 private:
  BPlusTree<size_t, Train> TrainMap;
  BPlusTree<id_date, train_seat> SeatMap;
  MultiBPlusTree<size_t, my_string<20>, station_train> StationPass;
  MultiBPlusTree<size_t, int, order> OrderInfo;
  MultiBPlusTree<id_date, int, pending> PendingInfo;
 public:
  TrainSystem(const std::string &name_1, const std::string &name_2,
              const std::string &name_3, const std::string &name_4,
              const std::string &name_5, const std::string &name_6,
              const std::string &name_7, const std::string &name_8,
              const std::string &name_9, const std::string &name_10) : TrainMap(name_1, name_2),
                                                                       SeatMap(name_3, name_4),
                                                                       StationPass(name_5, name_6),
                                                                       OrderInfo(name_7, name_8),
                                                                       PendingInfo(name_9, name_10) {}
  bool add_train(const std::string &id, int station_num, int seat_num, std::string *station,
                 const int *price, int start_time, const int *travel_times,
                 const int *stopover_times, Date begin_date, Date end_date, char _type) {
    size_t train_key = MyHash(id);
    if (TrainMap.find(train_key).stationNum > 0) return false;
    Train to_add;
    to_add.trainID = id, to_add.stationNum = station_num, to_add.seatNum = seat_num, to_add.startTime = start_time,
    to_add.type = _type;
    to_add.start_sale = begin_date, to_add.end_sale = end_date;
    for (int i = 1; i <= station_num; ++i) {
      to_add.stations[i] = station[i];
    }
    to_add.arrive_time[1] = 0, to_add.leave_time[1] = 0;
    for (int i = 2; i <= station_num; ++i) {
      to_add.arrive_time[i] = to_add.leave_time[i - 1] + travel_times[i - 1];
      if (i != station_num) {
        to_add.leave_time[i] = to_add.arrive_time[i] + stopover_times[i - 1];
      } else {
        to_add.leave_time[i] = 0;
      }
    }
    to_add.prices_sum[1] = 0;
    for (int i = 2; i <= station_num; ++i) {
      to_add.prices_sum[i] = to_add.prices_sum[i - 1] + price[i - 1];
    }
    TrainMap.insert(train_key, to_add);
    return true;
  }

  bool delete_train(const std::string &train_id) {
    size_t train_key = MyHash(train_id);
    Train res = TrainMap.find(train_key);
    if (res.stationNum == 0 || res.release_state) return false;
    TrainMap.erase(train_key);
    return true;
  }

  bool release_train(const std::string &train_id) {
    size_t train_key = MyHash(train_id);
    Train res = TrainMap.find(train_key);
    if (res.stationNum == 0 || res.release_state) return false;
    res.release_state = true;
    TrainMap.modify(train_key, res);
    for (int i = 1; i <= res.stationNum; ++i) {
      StationPass.insert(MyHash(res.stations[i]), res.trainID, station_train(res.leave_time[i], res.arrive_time[i],
                                                                             res.prices_sum[i], res.trainID,
                                                                             i, res.startTime));
    }
    return true;
  }

  std::string query_train(const Date &date, const std::string &train_id) {
    size_t train_key = MyHash(train_id);
    Train ret = TrainMap.find(train_key);
    if (ret.stationNum == 0 || date < ret.start_sale || ret.end_sale < date) {
      return "-1";
    } else {
      std::string ans;
      train_seat the_seat = SeatMap.find(id_date(train_key, date));
      if (!the_seat.station_num) {
        for (int i = 0; i < ret.stationNum; ++i) {
          the_seat.seat[i] = ret.seatNum;
        }
        the_seat.seat_num = ret.seatNum, the_seat.station_num = ret.stationNum;
      }
      ans += (std::string) ret.trainID + ' ' + ret.type;
      for (int i = 1; i <= ret.stationNum; ++i) {
        ans += '\n' + (std::string) ret.stations[i] + ' ';
        if (i == 1) {
          ans += "xx-xx xx:xx";
        } else {
          ans += (std::string) (Time(date) + ret.startTime + ret.arrive_time[i]);
        }
        ans += " -> ";
        if (i == ret.stationNum) {
          ans += "xx-xx xx:xx";
        } else {
          ans += (std::string) (Time(date) + ret.startTime + ret.leave_time[i]);
        }
        ans += ' ' + std::to_string(ret.prices_sum[i]) + ' ';
        if (i == ret.stationNum) {
          ans += 'x';
        } else {
          if (ret.release_state) {
            ans += std::to_string(the_seat.seat[i - 1]);
          } else {
            ans += std::to_string(ret.seatNum);
          }
        }
      }
      return ans;
    }
  }

  std::string query_ticket(const std::string &start, const std::string &end, const Date &date, bool order) {
    vector<station_train> depart_list = StationPass.find(MyHash(start)),
        arrive_list = StationPass.find(MyHash(end));
    vector<ticket_info> tickets;
    for (auto iter_1 = depart_list.begin(), iter_2 = arrive_list.begin(); iter_1 != depart_list.end(); ++iter_1) {
      for (; iter_2 != arrive_list.end() && (*iter_2).train_id < (*iter_1).train_id; ++iter_2);
      if (iter_2 == arrive_list.end()) {
        break;
      }
      if ((*iter_1).train_id == (*iter_2).train_id && (*iter_1).rank < (*iter_2).rank) { // right version
        Train ret = TrainMap.find((*iter_1).id_key);
        // judging the validity of date
        Time start_left(ret.start_sale, ret.startTime / 60, ret.startTime % 60),
            start_right(ret.end_sale, ret.startTime / 60, ret.startTime % 60);
        start_left += (*iter_1).leave, start_right += (*iter_1).leave;
        if (leq_day(start_left, date) && geq_day(start_right, date) && ret.release_state) { // totally right
          Date real_start = ret.start_sale + (date - start_left.day);
          train_seat the_seat = SeatMap.find(id_date(MyHash(ret.trainID), real_start));
          if (!the_seat.station_num) {
            for (int i = 0; i < ret.stationNum; ++i) {
              the_seat.seat[i] = ret.seatNum;
            }
            the_seat.seat_num = ret.seatNum, the_seat.station_num = ret.stationNum;
          }
          tickets.push_back(ticket_info(ret.trainID,
                                        Time(date, start_left.now / 60, start_left.now % 60),
                                        Time(date, start_left.now / 60, start_left.now % 60)
                                            + ((*iter_2).arrive - (*iter_1).leave),
                                        (*iter_2).prices_sum - (*iter_1).prices_sum,
                                        available(the_seat, (*iter_1).rank, (*iter_2).rank - 1),
                                        (*iter_2).arrive - (*iter_1).leave));
        }
      }
    }
    if (order) {
      sort(tickets.begin(), tickets.end(), TimeCmp);
    } else {
      sort(tickets.begin(), tickets.end(), CostCmp);
    }
    std::string out = std::to_string(tickets.size());
    for (auto iter = tickets.begin(); iter != tickets.end(); ++iter) {
      out +=
          '\n' + (std::string) (*iter).train_id + ' ' + start + ' ' + std::string((*iter).depart) + " -> " + end + ' '
              + std::string((*iter).arrive) + ' ' + std::to_string((*iter).price) + ' ' + std::to_string((*iter).seat);
    }
    return out;
  }

  std::string query_transfer(const std::string &start, const std::string &end, const Date &date, bool order) {
    bool found = false;
    std::string ret;
    vector<station_train> s_list = StationPass.find(MyHash(start));
    vector<station_train> t_list = StationPass.find(MyHash(end));
    map<my_string<20>, Train> end_list; // reduce redundant I/O
    map<id_date, train_seat> end_trains;
    int best_time = 2147483647, best_cost = 2147483647;
    std::string mid_transport;
    if (t_list.empty()) {
      return "0";
    }
    ticket_info s_ticket, t_ticket;
    for (auto s_iter : s_list) {
      Train train_s(TrainMap.find(s_iter.id_key)), train_t;
      Time left(train_s.start_sale, train_s.startTime / 60, train_s.startTime % 60);
      Time right(train_s.end_sale, train_s.startTime / 60, train_s.startTime % 60);
      left += s_iter.leave, right += s_iter.leave;
      if (!leq_day(left, date) || !geq_day(right, date)) continue;
      Date s_start = train_s.start_sale + (date - left.day);
      train_seat s_seat = SeatMap.find(id_date(s_iter.id_key, train_s.start_sale + (date - left.day))), t_seat;
      if (!s_seat.station_num) {
        for (int i = 0; i < train_s.stationNum; ++i) {
          s_seat.seat[i] = train_s.seatNum;
        }
        s_seat.station_num = train_s.stationNum, s_seat.seat_num = train_s.seatNum;
      }
      map<std::string, int> station_list;
      for (int i = s_iter.rank + 1; i <= train_s.stationNum; ++i) {
        station_list[std::string(train_s.stations[i])] = i;
      }
      for (auto t_iter : t_list) {
        if (s_iter.train_id == t_iter.train_id) continue;
        // must have one transfer
        if (end_list.find(t_iter.train_id) == end_list.end()) { // read from disk
          train_t = TrainMap.find(t_iter.id_key);
          end_list[t_iter.train_id] = train_t;
        } else {
          train_t = end_list[t_iter.train_id];
        }
        for (int i = 1; i < t_iter.rank; ++i) { // enumerate the transfer station
          std::string mid = std::string(train_t.stations[i]);
          if (station_list.find(mid) == station_list.end()) continue;
          // check whether train_t leaves after train_s arrives
          // by calculating the earliest time when train_t leaves
          Time s_arrive(date, left.now / 60, left.now % 60);
          s_arrive += train_s.arrive_time[station_list[mid]] - train_s.leave_time[s_iter.rank];
          Time t_right(train_t.end_sale, train_t.startTime / 60, train_t.startTime % 60);
          Time t_left(train_t.start_sale, train_t.startTime / 60, train_t.startTime % 60);
          t_right += train_t.leave_time[i], t_left += train_t.leave_time[i];
          if (t_right < s_arrive) continue;
          Time optimal(s_arrive);
          optimal += mod_minus(t_right, s_arrive);
          if (optimal < t_left) optimal = t_left;
          Date t_start = train_t.start_sale + (optimal.day - t_left.day);
          int least_time = optimal - s_arrive - train_s.leave_time[s_iter.rank] + train_s.arrive_time[station_list[mid]]
              + train_t.arrive_time[t_iter.rank] - train_t.leave_time[i];
          int now_cost = train_s.prices_sum[station_list[mid]] - s_iter.prices_sum
              + t_iter.prices_sum - train_t.prices_sum[i];
          bool better = false;
          id_date todo(t_iter.id_key, t_start);
          if (end_trains.find(todo) != end_trains.end()) {
            t_seat = end_trains[todo];
          } else {
            t_seat = SeatMap.find(todo);
            if (!t_seat.station_num) {
              for (int j = 0; j < train_t.stationNum; ++j) {
                t_seat.seat[j] = train_t.seatNum;
              }
              t_seat.station_num = train_t.stationNum, t_seat.seat_num = train_t.seatNum;
            }
            end_trains[todo] = t_seat;
          }
          // begin comparing
          found = true;
          if (order) {
            if (best_time == least_time) {
              if (best_cost == now_cost) {
                if (s_ticket.train_id == s_iter.train_id) {
                  better = t_iter.train_id < t_ticket.train_id;
                } else {
                  better = s_iter.train_id < s_ticket.train_id;
                }
              } else {
                better = now_cost < best_cost;
              }
            } else {
              better = least_time < best_time;
            }
          } else {
            if (best_cost == now_cost) {
              if (best_time == least_time) {
                if (s_ticket.train_id == s_iter.train_id) {
                  better = t_iter.train_id < t_ticket.train_id;
                } else {
                  better = s_iter.train_id < s_ticket.train_id;
                }
              } else {
                better = least_time < best_time;
              }
            } else {
              better = now_cost < best_cost;
            }
          }
          if (better) {
            best_time = least_time, best_cost = now_cost;
            mid_transport = mid;
            s_ticket = ticket_info(s_iter.train_id,
                                   Time(date, left.now / 60, left.now % 60),
                                   Time(date, left.now / 60, left.now % 60)
                                       + (-train_s.leave_time[s_iter.rank] + train_s.arrive_time[station_list[mid]]),
                                   train_s.prices_sum[station_list[mid]] - s_iter.prices_sum,
                                   available(s_seat, s_iter.rank, station_list[mid] - 1),
                                   0);
            t_ticket = ticket_info(t_iter.train_id,
                                   optimal,
                                   optimal + (train_t.arrive_time[t_iter.rank] - train_t.leave_time[i]),
                                   t_iter.prices_sum - train_t.prices_sum[i],
                                   available(t_seat, i, t_iter.rank - 1),
                                   0);
          }
        }
      }
    }
    if (found) {
      ret = std::string(s_ticket.train_id) + ' ' + start + ' ' + std::string(s_ticket.depart) + " -> " + mid_transport
          + ' '
          + std::string(s_ticket.arrive) + ' ' + std::to_string(s_ticket.price) + ' ' + std::to_string((s_ticket.seat));
      ret += '\n' + std::string(t_ticket.train_id) + ' ' + mid_transport + ' ' + std::string(t_ticket.depart) + " -> "
          + end + ' '
          + std::string(t_ticket.arrive) + ' ' + std::to_string(t_ticket.price) + ' ' + std::to_string((t_ticket.seat));
    } else {
      ret = "0";
    }
    return ret;
  }

  std::string buy_ticket(const std::string &username, const std::string &trainID, const Date &date,
                         const std::string &from, const std::string &to, bool wait, int num, int time) {
    station_train leave = StationPass.find_unique(MyHash(from), trainID);
    station_train arrive = StationPass.find_unique(MyHash(to), trainID);
    Train ret = TrainMap.find(MyHash(trainID));
    if (ret.seatNum == 0 || leave.rank == 0 || arrive.rank == 0 || leave.rank >= arrive.rank || ret.seatNum < num) {
      return "-1";
    }
    Time start_left(ret.start_sale, ret.startTime / 60, ret.startTime % 60),
        start_right(ret.end_sale, ret.startTime / 60, ret.startTime % 60);
    start_left += leave.leave, start_right += leave.leave;
    if (leq_day(start_left, date) && geq_day(start_right, date)) { // valid buy
      Date real_start = ret.start_sale + (date - start_left.day); // the real starting date
      id_date todo(MyHash(trainID), real_start);
      train_seat seat_info = SeatMap.find(todo);
      bool is_empty = false;
      if (!seat_info.station_num) {
        is_empty = true;
        for (int i = 0; i < ret.stationNum; ++i) {
          seat_info.seat[i] = ret.seatNum;
        }
        seat_info.station_num = ret.stationNum, seat_info.seat_num = ret.seatNum;
      }
      if (available(seat_info, leave.rank, arrive.rank - 1) >= num) { // can satisfy need
        if (!is_empty) SeatMap.erase(todo);
        satisfy_order(seat_info, leave.rank, arrive.rank - 1, num);
        SeatMap.insert(todo, seat_info);
        Time start_time(date, start_left.now / 60, start_left.now % 60);
        OrderInfo.insert(MyHash(username), -time + 2147483647, order(Success,
                                                                     arrive.prices_sum - leave.prices_sum,
                                                                     num,
                                                                     time,
                                                                     leave.rank,
                                                                     arrive.rank,
                                                                     trainID,
                                                                     from,
                                                                     to,
                                                                     start_time,
                                                                     start_time + (arrive.arrive - leave.leave),
                                                                     real_start));
        return std::to_string(num * (arrive.prices_sum - leave.prices_sum));
      } else {
        if (wait) {
          Time start_time(date, start_left.now / 60, start_left.now % 60);
          OrderInfo.insert(MyHash(username), -time + 2147483647,
                           order(Pending, arrive.prices_sum - leave.prices_sum,
                                 num, time, leave.rank, arrive.rank, trainID, from, to, start_time,
                                 start_time + (arrive.arrive - leave.leave), real_start));
          PendingInfo.insert(todo, time,
                             pending(num, time, leave.rank, arrive.rank, username));
          return "queue";
        } else { // can't buy tickets
          return "-1";
        }
      }
    } else {
      return "-1";
    }
  }

  std::string query_order(const std::string &username) {
    vector<order> ans = OrderInfo.find(MyHash(username));
    std::string ret = std::to_string(ans.size());
    for (auto iter : ans) {
      ret += '\n' + std::string(iter);
    }
    return ret;
  }

  bool refund_ticket(const std::string &username, int num) {
    size_t user_key = MyHash(username);
    vector<order> ret = OrderInfo.find(user_key);
    if (ret.size() < num || ret[num - 1].state == Refunded) return false;
    order target(ret[num - 1]);
    TicketState origin = target.state;
    target.state = Refunded;
    OrderInfo.modify(user_key, -target.time_stamp + 2147483647, target);
    id_date todo(MyHash(target.train_id), target.first_leave);
    if (origin == Pending) { // delete it from PendingInfo
      PendingInfo.erase(todo, target.time_stamp);
    } else { // return tickets and get successor
      train_seat seat_info = SeatMap.find(todo); // obviously it has been inserted before
      satisfy_order(seat_info, target.rank_s, target.rank_e - 1, -target.num);
      vector<pending> wait_list = PendingInfo.find(todo);
      for (auto iter : wait_list) {
        if (available(seat_info, iter.start_rank, iter.end_rank - 1) >= iter.num) { // can fill the need
          size_t iter_hash = MyHash(iter.username);
          target = OrderInfo.find_unique(iter_hash, -iter.time_stamp + 2147483647);
          target.state = Success;
          satisfy_order(seat_info, iter.start_rank, iter.end_rank - 1, iter.num);
          OrderInfo.modify(iter_hash, -iter.time_stamp + 2147483647, target);
          PendingInfo.erase(todo, iter.time_stamp);
        }
      }
      SeatMap.modify(todo, seat_info);
    }
    return true;
  }

  void clean() {
    TrainMap.clear(), SeatMap.clear(), StationPass.clear(), OrderInfo.clear(), PendingInfo.clear();
  }
};

#endif