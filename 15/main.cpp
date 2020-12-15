#include <iostream>
#include <map>
#include <vector>
#include <string>
#include <fstream>
#include <algorithm>
#include <utility>
#include <deque>
#include <queue>

using std::cout;
using std::endl;
using std::string;

std::vector<unsigned long> parse_numbers(const string& in)
{
  std::vector<unsigned long> out;
  auto begin = 0;
  auto end = in.find(",",0);
  while(end != string::npos)
    {
      out.push_back(std::stoul(in.substr(begin, end-begin)));
      begin = end+1;
      end = in.find(",", begin);
    };
  out.push_back(std::stoul(in.substr(begin, end-begin)));
  return out;
};

struct game
{
  unsigned long turn = 0;
  std::queue<unsigned long> _start_numbers;
  std::map<unsigned long, std::deque<unsigned long>> history;
  unsigned long last_spoken = -1;
  
  game(std::vector<unsigned long> start_numbers)
  {
    for(auto& n : start_numbers)
      _start_numbers.push(n);
  };
  
  unsigned long do_turn()
  {
    if(!_start_numbers.empty())
      {
	//cout << "not empty" << endl;
	unsigned long val = _start_numbers.front();
	_start_numbers.pop();
	history[val].push_back(turn++);
	last_spoken = val;
	return val;
      }

    auto numhist = history.find(last_spoken);
    if(numhist == history.end() || numhist->second.size() == 1)
      {
	//cout << "no history" << endl;
	unsigned long val = 0;
	history[val].push_back(turn++);
	last_spoken = val;
	return val;
      };

    //cout << "history size: " << numhist->second.size() << endl;
    auto turn_before_that = numhist->second.front();
    numhist->second.pop_front();
    auto most_recent_turn = numhist->second.front();
    auto val = (most_recent_turn - turn_before_that);
    history[val].push_back(turn++);
    last_spoken = val;
    return val;
    
    
    return -1;
  }
};


int main(int argc, char** argv)
{
  std::ifstream ifs("input.txt");
  string line;
  std::getline(ifs, line);

  //string line("1,3,2");
  auto nmbrs = parse_numbers(line);

  for(int n : nmbrs)
    cout << n << ",";
  cout << endl;

  game gm(nmbrs);
  // for(int i=0; i < 2020; i++)
  //   cout << gm.do_turn() << ", ";
  // cout << endl;
  unsigned long val =0;
  for(int i=0; i < 30000000; i++)
    val = gm.do_turn();

  cout << "val: " << val << endl;
  

}
