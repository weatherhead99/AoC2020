#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <algorithm>
#include <set>
#include <tuple>
#include <numeric>

using std::string;
using std::vector;
using std::cout;
using std::endl;
using std::tuple;

template<typename C, typename V=typename C::value_type,
	 typename I=typename C::const_iterator>
std::set<V> calculate_pairwise_sums(I&& start, I&& end)
{
  std::set<V> out;
  //  auto it_size = std::distance(start, end);
  //  out.reserve(it_size  * (it_size-1));
  auto it1 = start;
  while(it1 != end)
    {
      for(auto it2 = start; it2!=end; it2++)
	{
	  if(it2 == it1)
	     continue;
	  out.insert((*it1) + (*it2));
	}
      it1++;
    };
  
  return out;
}


template<typename C, typename V=typename C::value_type,
	 typename I=typename C::const_iterator>
bool contains(I&& start, I&& end, V&& val)
{
  auto loc = std::find(start,end, val);
  if(loc != end)
    return true;
  return false;
};

template<typename C, typename V=typename C::value_type,
	 typename It=typename C::const_iterator>
tuple<It, It> find_sum_range(const C& container, V target)
{
  auto range_start = container.cbegin();
  auto range_end = range_start +1;
  while(true)
    {
      range_end = range_start + 1;
      while(range_end != container.cend())
	{
	  V sum = std::accumulate(range_start, range_end,0);
	  if(sum == target)
	    goto breakout;
	  range_end++;
	}
      range_start++;
    }
 breakout:
  return std::make_tuple(range_start, range_end); 
};

int main(int argc, char** argv)
{
  std::ifstream ifs("input.txt");
  string line;
  vector<unsigned long> vals;
  while(std::getline(ifs, line))
    vals.push_back(std::stoul(line));

  int preamble_size = 25;

  auto preamble_begin = vals.cbegin();
  auto preamble_end = preamble_begin + preamble_size + 1;
  auto valit = preamble_end;
  unsigned long invalid_value = 0;
  while(valit != vals.cend())
    {
      auto position = std::distance(vals.cbegin(), valit);
      auto preamble_sums = calculate_pairwise_sums<decltype(vals)>(preamble_begin, preamble_end);
      
      if(!contains<decltype(vals)>(preamble_sums.cbegin(), preamble_sums.cend(),
				   *valit))
	{
	  cout << "value: " << *valit << ", at position: " << position <<  " is not a valid preamble sum" << endl;
	  invalid_value = *valit;
	  break;
	};
      valit++;
      preamble_begin++;
      preamble_end++;

    };

  cout << "invalid value: " << invalid_value << endl;

  auto [start, end] = find_sum_range(vals, invalid_value);
  cout << "start of range value: " << *start << endl;;
  cout << "end of range value: " << *end << endl;
  auto maxit = std::max_element(start, end);
  auto minit = std::min_element(start, end);
  auto sm = *maxit + *minit;
  cout << "sum of max and min value: " << sm << endl;
  
}
