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

//turns out this IS substantially faster than above version
template<typename C, typename V=typename C::value_type,
	 typename I=typename C::const_iterator>
std::set<V> calculate_pairwise_sums_sortfirst(I&& start, I&& end)
{
  std::set<V> out;
  C copy(start, end);
  std::sort(copy.begin(), copy.end());
  auto it1= copy.cbegin();
  while(it1 != copy.cend())
    {
      for(auto it2 = it1+1; it2 != copy.cend(); it2++)
	{
	  out.insert( (*it1) + (*it2));
	}
      it1++;
    }

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
	  V sum = 0;
	  for(auto rangeit = range_start; rangeit != range_end; rangeit++)
	    {
	      sum += *rangeit;
	      if(sum > target)
	  	break;
	    }  
	  if(sum == target)
	    goto breakout;
	  range_end++;
	}
      range_start++;
    }
 breakout:
  return std::make_tuple(range_start, range_end); 
};


template<typename C, typename V=typename C::value_type>
V find_invalid_value(const C& container, int preamble_size)
{
  auto preamble_begin = container.cbegin();
  auto preamble_end = preamble_begin + preamble_size +1;
  auto valit = preamble_end;
  V invalid_value = 0;
  while(valit != container.cend())
    {
      auto preamble_sums = calculate_pairwise_sums_sortfirst<C>(preamble_begin, preamble_end);
      if(!contains<decltype(container)>(preamble_sums.cbegin(), preamble_sums.cend(), *valit))
	{
	  invalid_value = *valit;
	  break;
	}
      valit++;
      preamble_begin++;
      preamble_end++;
    }
  return invalid_value;
};

int main(int argc, char** argv)
{
  std::ifstream ifs("input.txt");
  string line;
  vector<unsigned long> vals;
  while(std::getline(ifs, line))
    vals.push_back(std::stoul(line));

  auto invalid_value = find_invalid_value(vals, 25);
  cout << "invalid value: " << invalid_value << endl;
  
  auto [start, end] = find_sum_range(vals, invalid_value);
  cout << "start of range value: " << *start << endl;;
  cout << "end of range value: " << *end << endl;
  auto maxit = std::max_element(start, end);
  auto minit = std::min_element(start, end);
  auto sm = *maxit + *minit;
  cout << "sum of max and min value: " << sm << endl;
  
}
