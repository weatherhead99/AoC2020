#pragma once
#include <algorithm>
#include <numeric>
#include <vector>
#include <iostream>
using std::cout;
using std::endl;

template<typename K>
K factorial(K in)
{
  K out = in;
  for(K i = in-1 ; i > 0; i--)
    out *=i;
  return out;
}

template<typename K>
K comb(K n, K r)
{
  if(r > n)
    return 0;
  if(r==0)
    return 1;
  if(r==n)
    return 1;
  return factorial(n) / (factorial(r) * factorial(n-r));
};

template<typename K>
K sum_of_combs(K n, K maxr)
{
  K out = 0;
  for(int i=0; i <= maxr ; i++)
    {
      out += comb(n,i);
    };
  return out;
};


template<typename C, typename V = typename C::value_type,
	 typename It=typename C::const_iterator>
C get_diffs(It&& start, It&& end, int order=1)
{
  C diffs;
  diffs.resize(std::distance(start,end));
  std::adjacent_difference(start,end,diffs.begin());
  for(int i=1; i <order; i++)
    std::adjacent_difference(diffs.begin(),diffs.end(), diffs.begin());
  return diffs;
 
};


template<typename C, typename V=typename C::value_type>
std::vector<int> removal_run_lengths(const C& in)
{
  //find consecutive ranges
  //number of digits that can be removed from
  //any consecutive range is 2 less than the length
  //of the range, hence 1 less than the length of
  // the continuous 1 diff vector
  
  std::vector<int> rangelengths;
  auto diffs = get_diffs<C>(in.begin(), in.end());
  cout << "len diffs: " << diffs.size() << endl;
  cout << "len orig: " << in.size() << endl;
  int rangelen = 0;
  for(auto dit = diffs.begin() +1 ; dit != diffs.end(); dit++)
    cout << *dit << ",";
  cout << endl;
  for(auto dit = diffs.begin() +1; dit != diffs.end(); dit++)
    {
      //      cout << *dit << ", ";
      if(*dit != 1)
	{
	  //end of a consecutive range
	  if(rangelen > 1)
	    rangelengths.push_back(rangelen -1);
	  rangelen = 0;
	  continue;
	}
      rangelen++;
    }
  if(rangelen >0)
    rangelengths.push_back(rangelen);
  
  cout << endl;
  for(auto& v: rangelengths)
    cout << v << ", ";
  cout << endl;

  
  return rangelengths;
};
