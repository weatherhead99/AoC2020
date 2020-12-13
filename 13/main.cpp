#include <iostream>
#include <tuple>
#include <vector>
#include <string>
#include <algorithm>
#include <fstream>
#include <cmath>

using bus_timetable = std::tuple<unsigned long, std::vector<unsigned long>> ;

using std::cout;
using std::endl;

void lrstrip(std::string& s)
{
  s.erase(s.begin(), std::find_if(s.begin(), s.end(),[](unsigned char uc){return !std::isspace(uc);}));
  s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char uc){return !std::isspace(uc);}).base(), s.end());
}


template<typename Stream>
bus_timetable parse_timetable(Stream& strm)
{
  std::string line;
  std::getline(strm, line);
  lrstrip(line);
  int tstamp = std::stoul(line);

  std::vector<unsigned long> buses;
  std::getline(strm, line);
  std::size_t startpos = 0;
  auto delimpos = line.find(",");
  while(delimpos != std::string::npos)
    {
      auto busnum = line.substr(startpos, delimpos - startpos);
      if(busnum != "x")
	{
	  buses.push_back(std::stoul(busnum));
	}

      startpos = delimpos +1;
      delimpos = line.find(",",startpos);
     
    }

  return {tstamp, buses};
  
}

unsigned long get_next_highest_time(unsigned long tstamp, unsigned long busnum)
{
  return (std::ceil(tstamp/busnum) +1) * busnum;
};

int main(int argc, char** argv)
{
  cout << "hello" << endl;
  std::ifstream ifs("input.txt");

  auto [tstamp, buses] = parse_timetable(ifs);
  cout << "target timestamp: " << tstamp << endl;

  for(auto&b : buses)
    cout << b << ", ";
  cout << endl;
  
  std::vector<unsigned long> next_possible_times;
  next_possible_times.resize(buses.size());
  std::transform(buses.begin(),buses.end(),
		 next_possible_times.begin(),
		[&tstamp](auto& v){return get_next_highest_time(tstamp, v);});

  for(auto& npt: next_possible_times)
    {
      cout << npt << ", ";
    }
  cout << endl;

  //now find all the ones after the timestamp
  auto newend = std::remove_if(next_possible_times.begin(),
			       next_possible_times.end(),
			       [&tstamp](auto& v){return v <= tstamp;});
  next_possible_times.erase(newend, next_possible_times.end());
  
  for(auto& npt: next_possible_times)
    cout << npt << ",";
  cout << endl;
	      
  auto bustime = std::min_element(next_possible_times.begin(), next_possible_times.end());
  auto busnum = buses.begin() + std::distance(next_possible_times.begin(), bustime);
  cout << "bus time: " << *bustime << endl;
  cout << "wait time: " << (*bustime - tstamp) << endl;
  cout << "mult: " << *busnum * (*bustime - tstamp) << endl;
}

//part 2 requires solving a chinese remainder theorem puzzle
