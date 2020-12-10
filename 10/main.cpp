#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <algorithm>
#include <numeric>
#include "aoc_math.hh"

using std::cout;
using std::endl;
using std::vector;
using std::string;

//part 2 algo description:
//1) find  sequences of consecutive numbers.
//2) for each run of consecutive numbers, the outer two can neve be removed, all the rest can in some combinatino
//3), calculate # of combinations by LENC1 + LENC2 + LENC3 + 1 where C is combinations (nCr = n! / (n!(n-r)!), store somewhere
//4) multiply all these together

template<typename C>
void print_array(const C& arr)
{
  for(auto& v: arr)
    cout << v << ",";
  cout << endl;
}


int main(int argc, char** argv)
{
  std::ifstream ifs("input.txt");
  string line;
  vector<short> ratings;
  //add the adaptor from the wall plug
  ratings.push_back(0);
  while(std::getline(ifs, line))
    {
      ratings.push_back(std::stoi(line));
    };
  cout << "there are: " << ratings.size() << "adaptors" << endl;

  //sort the ratings
  std::sort(ratings.begin(), ratings.end());

  //add the adaptor for the device
  ratings.push_back( *(ratings.end()-1) + 3);
  print_array(ratings);

  auto diffs = get_diffs<decltype(ratings)>(ratings.begin(), ratings.end(),1);
  auto ndiffs_1 = std::count_if(diffs.cbegin(), diffs.cend(),
   				[] (auto& v) { return v==1;});
  auto ndiffs_3 = std::count_if(diffs.cbegin(), diffs.cend(),
   				[] (auto&v) {return v==3;});
  cout << "ndiffs_1: " << ndiffs_1 << ", ndiffs_3: "<< ndiffs_3 <<endl;
  cout << "product: "<< ndiffs_1 * ndiffs_3 << endl;


  
  auto runlengths = removal_run_lengths(ratings);
  long int outval = 1;
  for(auto& r : runlengths)
    {
      cout << "r: " << r << endl;
      auto v=  sum_of_combs(r,2);
      cout << "combinations: " << v << endl;
      outval *= v;

    }

  cout << "outval: " << outval << endl;
  
}
