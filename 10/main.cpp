#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <algorithm>
#include <numeric>
#include <list>
#include "aoc_math.hh"

using std::cout;
using std::endl;
using std::vector;
using std::string;
using std::list;

//part 2 algo description:
//1) find 2nd order differences of sorted sequence
//2) location of 0s in 2nd order diff are numbers that can be removed
//3) for each run of 0s, calculate # of combinations by LENC1 + LENC2 + LENC3 + 1 where C is combinations, store somewhere
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

  // auto diffs = get_diffs<decltype(ratings)>(ratings.begin(), ratings.end(),1);
  // auto ndiffs_1 = std::count_if(diffs.cbegin(), diffs.cend(),
  // 				[] (auto& v) { return v==1;});
  // auto ndiffs_3 = std::count_if(diffs.cbegin(), diffs.cend(),
  // 				[] (auto&v) {return v==3;});
  // cout << "ndiffs_1: " << ndiffs_1 << ", ndiffs_3: "<< ndiffs_3 <<endl;
  // cout << "product: "<< ndiffs_1 * ndiffs_3 << endl;

  // cout << "sum of combs (4,1) -- (4,3): " << sum_of_combs(4,3) << endl;

  // auto diffs_2 = get_diffs<decltype(ratings)>(ratings.begin(), ratings.end(),2);
  // auto diffs_1 = get_diffs<decltype(ratings)>(ratings.begin(), ratings.end());


  //NOTE: this one shouldn't include the end values... I think
  //  ratings.resize(ratings.size() -1);

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
  
  // auto removal_positions = get_removal_positions(ratings);

  
  // print_array(diffs_1);
  // print_array(diffs_2);
  // print_array(removal_positions);

  // auto run_lengths = get_run_lengths<decltype(ratings)>(ratings);
  // print_array(ratings);
  // print_array(run_lengths);
  
}
