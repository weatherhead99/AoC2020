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


template<typename C, typename V = typename C::value_type>
std::vector<std::size_t> get_removal_positions(const C& adaptors)
{
  auto diff2 = get_diffs<C>(adaptors.begin(),adaptors.end(),2);
  std::vector<std::size_t> out;
  out.reserve(diff2.size());
  for(auto dit = ++diff2.begin(); dit != diff2.end(); dit++)
    {
      if(*dit ==0)
	out.push_back(std::distance(diff2.begin(), dit) -1);
    }
  return out;
};

template<typename C, typename V = typename C::value_type>
std::vector<int> get_run_lengths(const C& adaptors)
{
  auto removal_positions = get_removal_positions<C>(adaptors);
  auto diff_removals = get_diffs<C>(removal_positions.begin(), removal_positions.end());
  for(auto& d : diff_removals)
    cout << d << ",";
  cout << endl;

  
  std::vector<int> runlengths;
  int runlength = 0;
  for(auto dit = diff_removals.begin() +2; dit != diff_removals.end(); dit++)
    {
      cout << "*dit: " << *dit << endl;
      runlength++;
      cout << "runglength: " << runlength << endl;
      if(*dit  != 1)
	{
	  runlengths.push_back(runlength);
	  runlength = 0;
	  continue;
	}

    };
  return runlengths;
}


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
