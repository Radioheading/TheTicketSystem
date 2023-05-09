#include "../lib/utils.hpp"
#include "../lib/bpt.hpp"
#include "../lib/map.hpp"
#include "../lib/exceptions.hpp"

const int max_info = 101;

class Train {
 private:
  my_string<20> trainID;
  int stationNum = 0, seatNum = 0, startTime = 0;
  my_string<40> stations[max_info];
  int prices[max_info - 1], travelTimes[max_info - 1], stopoverTimes[max_info - 2];
  int start_sale = 0, end_sale = 0;
  char type;
};
