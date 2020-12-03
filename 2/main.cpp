#include <iostream>
#include <string>
#include <fstream>
#include <array>
#include <tuple>
#include <algorithm>

using std::string;
using std::cout;
using std::endl;


std::string delims = " :-";

auto tokenize_line(const std::string& line)
{

  std::size_t next = -1;
  std::size_t current;
  std::array<string, 4> result;
  std::size_t i =0;
  do
    {
      next = line.find_first_not_of(delims, next+1);
      if(next == string::npos) break;
      next -=1;
      current = next + 1;
      next = line.find_first_of(delims, current);
      result[i] = line.substr(current, next-current);
      i += 1;
    }
  while(next != string::npos);  
  return std::make_tuple(result[2][0], std::stoi(result[0]),
			 std::stoi(result[1]),result[3]);
};


bool check_password(char target, short unsigned min,
		    short unsigned max, const string& password)
{
  auto count = std::count(password.begin(), password.end(), target);
  if(count >= min && count <= max)
    return true;
  return false;
};


bool check_password_part2(char target, short unsigned firstpos,
			  short unsigned secondpos, const string& password)
{
  //LOGICAL XOR
  if(!(password[firstpos-1] == target) !=
     !(password[secondpos-1] == target))
    return true;
  return false;
};

int main(int argc, char** argv)
{
  cout << "loading file..." << endl;
  std::ifstream ifs("input.txt");
  string line;
  std::size_t count = 0;
  std::size_t count_part2 = 0;
  std::size_t total = 0;
  while(std::getline(ifs, line))
    {
      auto parsed = tokenize_line(line);      
      if(std::apply(check_password, parsed))
	count++;
      if(std::apply(check_password_part2, parsed))
	  count_part2++;
      total++;
    }
  cout  << "number which passed: " << count << endl;
  cout << "number which passed part 2:" << count_part2 << endl;
  cout << "total considered: " << total << endl;

}
