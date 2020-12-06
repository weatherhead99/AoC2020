#include <iostream>
#include <fstream>
#include <set>
#include <string>
#include <vector>
#include <numeric>
#include <algorithm>

using std::vector;
using std::string;
using std::cout;
using std::endl;

template<typename F>
std::vector<std::set<char>> parse_input(const string& fname, F&& f)
{
  std::ifstream ifs(fname);
  string line;
  std::set<char> current_entry;
  std::vector<std::set<char>> out;
  bool first_line = true;
  while(std::getline(ifs,line))
    {
      if(line.size() == 0)
	{
	  out.push_back(current_entry);
	  current_entry.clear();
	  first_line = true;
	}

      std::set<char> line_entry;
      for(auto& c: line)
	{
	  line_entry.insert(c);
	}
      f(current_entry, line_entry, first_line);      
      if(current_entry.size() > 0)
	first_line = false;
    }
  out.push_back(current_entry);
  return out;
}

void proc_part1(std::set<char>& current,
		const std::set<char>& line_entry,
		bool first_entry)
{
  std::copy(line_entry.cbegin(), line_entry.cend(),
	    std::inserter(current, current.begin()));

}

void proc_part2(std::set<char>& current,
		const std::set<char>& line_entry,
		bool first_entry)
{
  if(first_entry)
    {
      current = line_entry;
    }
  else
    {
      std::set<char> temp_entry;
      std::set_intersection(current.cbegin(), current.cend(),
			    line_entry.cbegin(), line_entry.cend(),
			    std::inserter(temp_entry, temp_entry.begin()));
      current = temp_entry;
    }
}


int sum_counts(const std::vector<std::set<char>>& in)
{
  auto sumcounts = std::accumulate(in.cbegin(), in.cend(),
				   0, [](int acc, const std::set<char>& s)
				   {return acc + s.size();});
  return sumcounts;
}
 

int main(int argc, char** argv)
{
  string input_file = "input.txt";
  auto entries = parse_input(input_file, proc_part1);

  cout << "number of groups: " << entries.size() << endl;
  cout << "count part 1: " << sum_counts(entries) << endl;
  
  auto entries2 = parse_input(input_file, proc_part2);
  cout << "count part 2: " << sum_counts(entries2) << endl;
  
}

