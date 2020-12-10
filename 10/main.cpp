#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <algorithm>
#include <numeric>
#include <list>

using std::cout;
using std::endl;
using std::vector;
using std::string;
using std::list;

int main(int argc, char** argv)
{
  std::ifstream ifs("input2.txt");
  string line;
  vector<unsigned short> ratings;
  while(std::getline(ifs, line))
    {
      ratings.push_back(std::stoi(line));
    };
  cout << "there are: " << ratings.size() << "adaptors" << endl;

  //sort the ratings
  std::sort(ratings.begin(), ratings.end());

  //add the adaptor for the device
  ratings.push_back( *(ratings.end()-1) + 3);

  for(auto& a : ratings)
    cout << a << ",";
  cout << endl;

  std::vector<unsigned short> diffs;
  diffs.resize(ratings.size());
  std::adjacent_difference(ratings.begin(), ratings.end(), diffs.begin());
  cout << "---------" << endl;
  for(auto& d: diffs)
    cout << (int) d << ",";
  cout << endl;

  auto ndiffs_1 = std::count_if(diffs.cbegin(), diffs.cend(),
				[] (auto& v) { return v==1;});
  auto ndiffs_3 = std::count_if(diffs.cbegin(), diffs.cend(),
				[] (auto&v) {return v==3;});
  cout << "ndiffs_1: " << ndiffs_1 << ", ndiffs_3: "<< ndiffs_3 <<endl;

  cout << "product: "<< ndiffs_1 * ndiffs_3 << endl;

  std::list<unsigned short> ratingslst(ratings.begin(), ratings.end());
  unsigned long total_arrangements = 0;
  for(int i=0; i < 5; i++)
    {
      for(auto& v : ratingslst)
	cout << v << ", ";
      cout << endl << "------------" << endl;

      std::vector<unsigned short> diffs;
      diffs.resize(ratingslst.size());
      std::adjacent_difference(ratingslst.begin(), ratingslst.end(),
			       diffs.begin());

      for(auto& d : diffs)
	cout << d  << ", ";
      cout << endl << "-----------" << endl;

      auto n_arrange = std::count_if(diffs.cbegin() +1, diffs.cend(),
				     [](auto& d){return d < 3;});

      cout << "n_arrange: " << n_arrange << endl;
      total_arrangements += n_arrange;

      if(n_arrange ==0)
	break;
      
      auto diffit = diffs.begin()+1;
      auto first_rating = ++ratingslst.begin();
      auto last_rating = --ratingslst.end();

      cout << "ratings size before: " << ratingslst.size() << endl;
      auto removed = std::remove_if(first_rating, last_rating,
				    [&diffit](auto& v)
				    {return (*diffit++) < 3 ? true : false;});

      ratingslst.erase(removed, ratingslst.end());
      cout << "ratings size after: " << ratingslst.size() << endl;
    }

  cout << "total arrangements: " << total_arrangements << endl;
}
