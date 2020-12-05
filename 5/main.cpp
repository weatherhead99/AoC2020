#include <iostream>
#include <array>
#include <cmath>
#include <string>
#include <algorithm>
#include <fstream>
#include <vector>
#include <numeric>

using std::cout;
using std::endl;
using std::string;

template<char L, char U>
bool test_character(char c)
{
  if(c == L) return false;
  else if (c==U) return true;
  else  throw std::runtime_error("invalid letter! " + c);
}

std::array<unsigned char, 10> parse_line(const string& s)
{
  std::array<unsigned char, 10> out;
  std::transform(s.cbegin(), s.cbegin() + 7, out.begin(),
		 test_character<'F','B'>);
  std::transform(s.cbegin() + 7, s.cend(), out.begin() + 7,
		 test_character<'L','R'>);
  return out;
};


std::array<int, 2> binary_split(int lower, int higher, bool top)
{
  int pivot = std::floor( (higher-lower) / 2.) + lower;
  if(top)
    return {pivot+1, higher};
  return {lower, pivot};
}

int find_seat(const string& s)
{
  auto ps = parse_line(s);
  auto psit = ps.cbegin();
  int l = 0;
  int h = 127;

  auto fun =  [&l, &h](bool b) { auto a = binary_split(l,h,b);
				 l = a[0];
				 h = a[1];};
  
  while(std::distance(ps.cbegin(), psit) < 7)
    {
      fun(*psit++);
    }
  if(l !=h )
    throw std::runtime_error("failed to converge binary search on row");
  auto row = l;

  l =0;
  h = 7;
  while(psit != ps.cend())
    {
      fun(*psit++);
    }

  if(l != h)
    throw std::runtime_error("failed to converge binary search on column");
  auto column = l;

  auto coord = row * 8 + column;
  return coord;
}
  

int main(int argc, char** argv)
{
  std::ifstream ifs("input.txt");
  string line;
  std::vector<int> coords;
  
  while(std::getline(ifs, line))
    {
      coords.push_back(find_seat(line));
    }

  cout << "number of seats parsed:  " << coords.size() << endl;
  auto  maxit = std::max_element(coords.cbegin(), coords.cend());
  cout << "max seat ID: " << *maxit << endl;

  std::sort(coords.begin(), coords.end());

  std::vector<int> diffs;
  diffs.resize(coords.size());
  std::adjacent_difference(coords.cbegin(), coords.cend(), diffs.begin());
  auto findit = std::find_if(diffs.cbegin(), diffs.cend(),
			     [](const int i){ return i> 1;});

  while(findit != diffs.cend())
    {
      cout << "missing seat id position: " << std::distance(diffs.cbegin(), findit) << endl;
      auto dist = std::distance(diffs.cbegin(), findit);
      auto missing_id = coords[dist] -1;
      cout << "missing ID: " << missing_id << endl;
      auto row = std::floor(missing_id /8);
      auto col = missing_id %8;
      cout << "row: " << row << ", col: " << col << endl;
      
      findit = std::find_if(findit+1, diffs.cend(),
			    [](const int i){return i>1;});
    }

  
  // string test = "FBFBBFFRLR";
  // string test2 = "BFFFBBFRRR";
  // auto coord = find_seat(test);
  // cout << "coord: "<< coord << endl;
  // cout << "test2: " << find_seat(test2) << endl;
}
