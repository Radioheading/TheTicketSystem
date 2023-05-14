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

struct Train {
  bool release_state = false;
  my_string<20> trainID{};
  int stationNum = 0, seatNum = 0, startTime = 0;
  my_string<40> stations[max_info]{};
  int prices_sum[max_info - 1]{};
  int leave_time[max_info - 1], arrive_time[max_info]{0};
  Date start_sale{}, end_sale{};
  char type{};

  friend bool operator<(const Train &cmp_1, const Train &cmp_2) {
    return cmp_1.trainID < cmp_2.trainID;
  }

  friend bool operator==(const Train &cmp_1, const Train &cmp_2) {
    return cmp_1.trainID == cmp_2.trainID;
  }

  Train &operator=(const Train &other) {
    if (&other == this) return *this;
    release_state = other.release_state, stationNum = other.stationNum, seatNum = other.seatNum, startTime =
        other.startTime;
    trainID = other.trainID, type = other.type;
    start_sale = other.start_sale, end_sale = other.end_sale;
    for (int i = 1; i <= stationNum; ++i) {
      stations[i] = other.stations[i];
    }
    for (int i = 1; i < stationNum; ++i) {
      prices_sum[i] = other.prices_sum[i], leave_time[i] = other.leave_time[i], arrive_time[i] = other.arrive_time[i];
    }
    return *this;
  }
};

struct train_seat {
  int seat[max_info], station_num = 0, seat_num = 0;
  Date start_date; // we store the day when the train departs
  friend bool operator==(const train_seat &cmp_1, const train_seat &cmp_2) {
    return cmp_1.start_date == cmp_2.start_date;
  }
  friend bool operator<(const train_seat &cmp_1, const train_seat &cmp_2) {
    return cmp_1.start_date < cmp_2.start_date;
  }
};

struct id_date {
  my_string<20> id;
  Date date; // the day when this train starts

  id_date() = default;

  id_date(const my_string<20> &_id, const Date &_date) : id(_id), date(_date) {}

  friend bool operator<(const id_date &cmp_1, const id_date &cmp_2) {
    return cmp_1.id < cmp_2.id || cmp_1.id == cmp_2.id && cmp_1.date < cmp_2.date;
  }

  friend bool operator==(const id_date &cmp_1, const id_date &cmp_2) {
    return cmp_1.id == cmp_2.id && cmp_1.date == cmp_2.date;
  }
};

int available(const train_seat &seat, int l, int r) {
  int ans;
  for (int i = l; i <= r; ++i) {
    if (i == l) {
      ans = seat.seat[l];
    } else {
      ans = std::min(seat.seat[l], ans);
    }
  }
  return ans;
}

struct station_train {
  int leave = 0, arrive = 0, prices_sum = 0, rank = 0, start_time = 0;
  my_string<20> train_id;
  station_train() = default;
  station_train(int _leave, int _arrive, int _sum, const my_string<20> &id, int _rank, int _start_time)
      : leave(_leave), arrive(_arrive), prices_sum(_sum), rank(_rank), start_time(_start_time),
        train_id(id) {};

  friend bool operator<(const station_train &cmp_1, const station_train &cmp_2) {
    return cmp_1.train_id < cmp_2.train_id;
  }
};

struct ticket_info {
  my_string<20> train_id;
  Time depart, arrive;
  int price, seat, duration;

  ticket_info(const my_string<20> &_train_id,
              Time _depart,
              Time _arrive,
              int _price,
              int _seat,
              int _duration)
      : train_id(_train_id),
        depart(std::move(_depart)),
        arrive(std::move(_arrive)),
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

struct 

class TrainSystem {
 private:
  BPlusTree<my_string<20>, Train> TrainMap;
  BPlusTree<id_date, train_seat> SeatMap;
  MultiBPlusTree<my_string<40>, station_train> StationPass;
 public:
 public:
  TrainSystem(const std::string &name_1,
              const std::string &name_2,
              const std::string &name_3,
              const std::string &name_4,
              const std::string &name_5,
              const std::string &name_6) : TrainMap(name_1, name_2),
                                           SeatMap(name_3, name_4),
                                           StationPass(name_5, name_6) {}
  bool add_train(const std::string &id,
                 int station_num,
                 int seat_num,
                 std::string *station,
                 const int *price,
                 int start_time,
                 const int *travel_times,
                 const int *stopover_times,
                 Date begin_date,
                 Date end_date,
                 char _type) {
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
      }
    }
    to_add.prices_sum[0] = 0;
    for (int i = 1; i < station_num; ++i) {
      to_add.prices_sum[i] = to_add.prices_sum[i - 1] + price[i];
    }
    TrainMap.insert(to_add.trainID, to_add);
    return true;
  }

  bool delete_train(const std::string &train_id) {
    my_string<20> id = train_id;
    Train res = TrainMap.find(train_id);
    if (res.stationNum == 0 || !res.release_state) return false;
    TrainMap.erase(train_id, res);
    return true;
  }

  bool release_train(const std::string &train_id) {
    my_string<20> id = train_id;
    Train res = TrainMap.find(train_id);
    if (res.stationNum == 0 || res.release_state) return false;
    TrainMap.erase(id, res), res.release_state = true, TrainMap.insert(id, res);
    for (int i = 1; i <= res.stationNum; ++i) {
      StationPass.insert(res.stations[i],
                         station_train(res.leave_time[i],
                                       res.arrive_time[i],
                                       res.prices_sum[i],
                                       res.trainID,
                                       i,
                                       res.startTime));
    }

    return true;
  }

  std::string query_train(const Date &date, const std::string &train_id) {
    my_string<20> id = train_id;
    Train ret = TrainMap.find(id);
    if (ret.stationNum == 0) {
      return "-1";
    } else {
      std::string ans;
      train_seat the_seat = SeatMap.find(id_date(id, date));
      ans += (std::string) ret.trainID + ' ' + ret.type;
      for (int i = 1; i <= ret.stationNum; ++i) {
        ans += '\n' + (std::string) ret.stations[i] + ' ';
        if (i == 1) {
          ans += "xx-xx xx:xx";
        } else {
          ans += (std::string) (Time(date) + ret.arrive_time[i]);
        }
        ans += " -> ";
        if (i == ret.stationNum) {
          ans += "xx-xx xx:xx";
        } else {
          ans += (std::string) (Time(date) + ret.leave_time[i]);
        }
        ans += ' ' + std::to_string(ret.prices_sum[i]) + ' ';
        if (ret.release_state) {
          ans += std::to_string(the_seat.seat[i]) + '\n';
        } else {
          ans += std::to_string(ret.seatNum) + '\n';
        }
      }
      return ans;
    }
  }

  std::string query_ticket(const std::string &start, const std::string &end, const Date &date, bool order) {
    my_string<40> depart(start), arrive(end);
    sjtu::vector<station_train> depart_list = StationPass.find(depart), arrive_list = StationPass.find(arrive);
    sjtu::vector<ticket_info> tickets;
    for (auto iter_1 = depart_list.begin(), iter_2 = arrive_list.begin(); iter_1 != depart_list.end(); ++iter_1) {
      for (; iter_2 != arrive_list.end() && (*iter_2).train_id < (*iter_1).train_id; ++iter_2);
      if (iter_2 == arrive_list.end()) {
        break;
      }
      if ((*iter_1).train_id == (*iter_2).train_id && (*iter_1).leave < (*iter_2).arrive) { // right version
        Train ret = TrainMap.find((*iter_1).train_id);
        // judging the validity of date
        Time start_left(ret.start_sale, ret.startTime / 60, ret.startTime % 60),
            start_right(ret.end_sale, ret.startTime / 60, ret.startTime % 60);
        start_left += (*iter_1).leave, start_right += (*iter_1).leave;
        if (leq_day(start_left, date) && geq_day(start_right, date) && ret.release_state) { // totally right
          Date real_start = ret.start_sale + (date - start_left.day);
          train_seat the_seat = SeatMap.find(id_date(ret.trainID, real_start));
          tickets.push_back(ticket_info(ret.trainID,
                                        Time(date, start_left.now / 60, start_left.now % 60),
                                        Time(date, start_left.now / 60, start_left.now % 60)
                                            + ((*iter_2).arrive - (*iter_1).leave),
                                        (*iter_2).prices_sum - (*iter_1).prices_sum,
                                        available(the_seat, (*iter_1).rank, (*iter_2).rank),
                                        (*iter_2).arrive - (*iter_1).leave));
        }
      }
    }
    // todo sorting the vector
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

  }

  std::string buy_ticket(const std::string &username,
                         const std::string &trainID,
                         const Date &date,
                         const std::string &from,
                         const std::string &to,
                         bool wait,
                         int num,
                         int time) {

  }

  std::string query_order(const std::string &username) {

  }

  bool refund_ticket(const std::string &username, int num) {

  }

  void clean() {

  }
};

#endif


