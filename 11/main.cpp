#include <iostream>
#include <fstream>
#include <vector>
#include <tuple>
#include <string>
#include <sstream>
#include <array>
#include <algorithm>
#include <numeric>
#include <functional>

using std::cout;
using std::endl;
using std::string;

enum class position_type : unsigned char { empty_seat, filled_seat, no_seat};

using Grid = std::tuple<std::vector<position_type>, int>;

template<typename Stream>
Grid parse_grid(Stream&& s)
{
  string line;
  int Nx = 0;
  int column = 0;
  std::vector<position_type> out;
  while(std::getline(s, line))
    {
      int linelen = line.size();
      if(Nx == 0)
	Nx = linelen;
      else if(Nx != linelen)
	throw std::logic_error("line lenght has changed!");

      out.reserve(out.size() + Nx);
      for(char c : line)
	{
	  switch(c)
	    {
	    case('#'): out.push_back(position_type::filled_seat); break;
	    case('L'): out.push_back(position_type::empty_seat); break;
	    case('.'): out.push_back(position_type::no_seat); break;
	    default: throw std::logic_error("invalid character in input");
	    }
	}
      column++;
      
    };
  
  return std::make_tuple(out, Nx);
};

std::tuple<int,int> idx_to_coord(int idx, const Grid& grid)
{
  auto nrows = std::get<1>(grid);
  auto row = idx / nrows;
  auto col = idx % nrows;
  
  return std::make_tuple(col, row);
}

int coord_to_idx(const std::tuple<int,int>& coord, const Grid& grid)
{
  auto [x,y] = coord;
  auto nrows = std::get<1>(grid);
  return y * nrows + x;
};


std::vector<position_type> get_neighbours(const Grid& grid, int idx)
{
  auto [vals, rows] = grid;
  int ncols = vals.size() / rows;
  auto [c, r] = idx_to_coord(idx, grid);
  std::vector<position_type> out;
  out.reserve(8);
  //  cout << "idx_col: " << c << " idx_row: " << r << endl;
  for(int i= c -1; i < c+1; i++)
    {
      if((i < 0) || (i >= ncols))
	  continue;
      for(int j = r-1; j < r+1; j++)
	{
	  if((j < 0) || (j >= rows))
	    continue;
	  auto thisidx = coord_to_idx({i,j},grid);
	  
	  //cout << "thisidx: " << thisidx << "idx: " << idx << endl;
	  if(thisidx == idx)
	    continue;
	  //cout << "i: " << i << " j: " << j << endl;
	  out.push_back(vals[thisidx]);
	  
	}
    }
  return out;
};


Grid round_update_part1(Grid& grid)
{
  Grid out;
  auto& [vals, nrows] = grid;
  std::get<0>(out) = vals;
  std::get<1>(out) = nrows;
  auto cols = vals.size() / nrows;
 for( int i=0 ; i < cols; i++)
    {
      for(int j=0; j < nrows; j++)
	{
	  //cout << "i: " << i << "j: "<< j  << endl;
	  auto idx = coord_to_idx({i,j}, grid);
	  auto thisval = vals[idx];
	  if(thisval != position_type::no_seat)
	    {
	      auto neighbours = get_neighbours(grid, idx);
	      // for(auto& n : neighbours)
	      // 	cout << (int) n << ", ";
	      // cout << endl;
	      auto n_occupied = std::count(neighbours.begin(), neighbours.end(),
					     position_type::filled_seat);
	      cout << " i: " << i << " j: " << j ;
	      cout << "n_neighbours: " << neighbours.size() << " n_occupied: " << n_occupied << endl;
	      if( (thisval == position_type::empty_seat) && (n_occupied == 0))
		std::get<0>(out)[idx] = position_type::filled_seat;
	      else if((thisval == position_type::filled_seat) && (n_occupied >= 4))
		std::get<0>(out)[idx] = position_type::empty_seat;
	    }
	}
      }
  return out;
}



void print_grid(Grid& grid)
{
  auto [vals, rows] = grid;
  auto cols = vals.size() / rows;
  
  for(int j = 0; j < rows; j++)
    {
      for(int i=0; i<  cols; i++)
	{
	  auto idx = coord_to_idx({i,j},grid);
	  switch(vals[idx])
	    {
	    case(position_type::filled_seat): cout << '#';break;
	    case(position_type::empty_seat) : cout << 'L';break;
	    case(position_type::no_seat) : cout << "."; break;
	    }
	}
      cout << endl;
    }

}


std::tuple<Grid, int> count_rounds_till_stability(Grid& grid)
{
  int rounds = 0;
  Grid gridcpy(grid);
  auto gridupd = round_update_part1(grid);
  while(true)
    {
      //cout << "round: " << rounds << endl;
      if(std::get<0>(gridupd) == std::get<0>(gridcpy))
	break;
      gridcpy = gridupd;
      gridupd = round_update_part1(gridupd);
      rounds++;
      //print_grid(gridupd);
      //cout << "---------" << endl;
      //print_grid(gridcpy);
    }

  return std::make_tuple(gridupd, rounds);
}


int main(int argc, char** argv)
{
  std::ifstream ifs("input2.txt");
  auto grid = parse_grid(ifs);
  auto[values, rows] = grid;

  cout << "number of columns: " << values.size() /rows << endl;
  cout << "number of rows: " << rows << endl;

  auto gridupd = round_update_part1(grid);
  cout << "grid updated" << endl;
  for(int i=0;i < 7; i++)
    {
      print_grid(gridupd);
      cout << "----------------------" << endl;
      gridupd = round_update_part1(gridupd);
    }
  
  
  // auto [gridupd, n_rounds] = count_rounds_till_stability(grid);
  // cout << "n_rounds: " << n_rounds << endl;
  // auto n_occupied = std::count(std::get<0>(gridupd).begin(), std::get<0>(gridupd).end(),
  // 			       position_type::filled_seat);
  // cout << "n_occupied: "<< n_occupied << endl;
 
  
 
};
