#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <tuple>
#include <array>

using std::vector;
using std::cout;
using std::endl;

std::vector<unsigned char> parse_line(const std::string& line)
{
  std::vector<unsigned char> out;
  out.resize(line.size());
  std::transform(line.begin(), line.end(),
		 out.begin(), [](char a) {
		   if(a == '.')
		     return 0;
		   else if(a=='#')
		     return 1;
		   else
		     {
		       cout << (int) a << endl;
		     throw a;}});

  return out;
};


//DON'T use vector<bool>!
auto read_grid_from_file()
{
  std::ifstream ifs("input.txt");
  std::vector<std::vector<unsigned char>> out;
  std::string line;
  while(std::getline(ifs, line))
    {
      out.push_back(parse_line(line));
    }
  return out;
};


int traverse_grid(int steps_i, int steps_j, const std::vector<std::vector<unsigned char>>& grid)
{
  int i=0;
  int j=0;
  int num_trees = 0;
  while(j < grid.size())
    {
      num_trees += grid[j][i];
      j += steps_j;
      i = (i+steps_i) % grid[0].size();
    }
  return num_trees;

};


int main(int argc, char** argv)
{
  cout << "parsing grid..." << endl;
  auto grid = read_grid_from_file();
  cout << "grid size is: "<< grid.size() << ", " << grid[0].size() << endl;
  cout << "part 1 number of trees..." << endl;
  cout << traverse_grid(3,1, grid) << endl;
  
  std::array<std::tuple<int,int>, 5> invals = {{{1,1}, {3,1}, {5,1}, {7,1}, {1,2}}};

  auto applfun = [&grid](int istep, int jstep){return traverse_grid(istep, jstep, grid);};
  long unsigned product = 1;
  for(auto& params: invals)
    {
      cout << "calculating for istep: " << std::get<0>(params) << ", jstep: " << std::get<1>(params) << endl;
      auto answer = std::apply(applfun, params);
      cout << "answer: " << answer << endl;
      product *= answer;
    };

  cout << "product: " << product << endl;
  
}
