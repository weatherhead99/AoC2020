#include <iostream>
#include <tuple>
#include <map>
#include <string>
#include <sstream>
#include <fstream>
#include <array>
#include <vector>
#include <algorithm>

using std::cout;
using std::endl;
using std::string;

using point = std::tuple<int, int>;

point locate_point(const string& s)
{
  std::istringstream iss(s);
  char c;
  char c2;
  int x = 0;
  int y = 0;
  while(iss.get(c))
    {
      switch(c)
	{
	case('e'): x +=1; break;
	case('w'): x -= 1; break;
	case('n'):
	  iss.get(c2); //know this will work
	  switch(c2)
	    {
	    case('e'): y += 1; break;
	    case('w'): y+=1; x -=1; break;
	    }
	  break;
	case('s'):
	  iss.get(c2);
	  switch(c2)
	    {
	    case('e') : x +=1; y -= 1; break;
	    case('w') : y-=1; break;
	    }
	  break;
	}
    }
  return std::make_tuple(x,y);

};

std::array<point, 6> get_neighbours(const point& centre)
{
  auto const& [x,y] = centre;
  std::array<point, 6> out {{
      {x-1, y},
	{x+1, y},
	  {x, y-1},
	    {x,y+1},
	      {x-1,y+1},
		{x+1, y-1}}
  };
  return out;
};



int main(int argc, char** argv)
{
  std::ifstream ifs("input.txt");
  string line;
  std::map<point, bool> tilemap;
  while(std::getline(ifs, line))
    {
      auto tgt_coord = locate_point(line);
      auto pos = tilemap.find(tgt_coord);
      if(pos != tilemap.end())
	{
	  pos->second = not pos->second;
	}
      else
	{
	  tilemap[tgt_coord] = true;
	}
    };


  //count number of black tiles
  int black_count = 0;
  int max_x = 0;
  int max_y = 0;
  for(auto& [k,v] : tilemap)
    {
      black_count += v;
      max_x = std::max(std::get<0>(k), max_x);
      max_y = std::max(std::get<1>(k), max_y);
    }
  cout << "black count: " << black_count << endl;
  cout << "max_x: " << max_x << endl;
  cout << "max_y: " << max_y << endl;

  //convert to a dense representation (it's only 14 x 14
  

  
};
