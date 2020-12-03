#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>

using std::string;
using std::cout;
using std::endl;
using std::vector;

int TOTAL = 2020;

int main(int argc, char** argv)
{
  cout << "loading file..." << endl;
  std::ifstream ifs("input.txt");
  std::string line;
  std::vector<int> values;
  while(std::getline(ifs, line))
    {
      values.push_back(std::stoi(line));
    }

  for(auto& v : values)
    {
      for(auto& v2: values)
	{
	  auto searchval = TOTAL - v - v2;
	  auto fit = std::find(values.begin(), values.end(), searchval);
	  if(fit != values.end())
	    {
	      cout << "found values: " << v << ", " << v2 << " and " << *fit << endl;
	      cout << "their product is: " << v2 *  v * (*fit) << endl;
	      std::exit(0);
	    }
	}
    }
  
};
