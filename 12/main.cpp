#include <iostream>
#include <tuple>
#include <vector>
#include <string>
#include <algorithm>
#include <fstream>
using std::string;

using std::cout;
using std::endl;

enum class commands : unsigned char { forward, north, east, south, west, left, right};
enum class facing : unsigned char { north, east, south, west};


int fastcos(int n_right_angles)
{
  if( std::abs((n_right_angles %2)) == 1)
    return 0;
  if(std::abs((n_right_angles /2)) %2 ==1)
    {
      return -1;
    }
  return 1;
}

int fastsin(int n_right_angles)
{
  if(n_right_angles %2 == 0)
    return 0;
  if(((n_right_angles-1)/2) %2 ==0)
    return 1;
  return -1;
}

std::tuple<int,int> facing_to_vec(facing f)
{
  //0 if north or south, 1 if east or west
  int a = static_cast<unsigned char>(f) % 2;
  //1 if north or east, -1 if south or west
  int b = static_cast<unsigned char>(f) <2 ? 1 : -1;
  return {a * b,  (!a) * b};
};

struct ship
{
public:
  ship() {};
  void do_command(const commands& command, const int arg)
  {
    auto& [x,y] = position;
    auto [dx, dy] = facing_to_vec(currentheading);
    cout << "dx: " << dx << " dy: " << dy << endl;
    cout << "arg: " << arg << endl;
    cout << "x: " << x << " y: " << y << endl;
    cout << "currentheading: " << (int) currentheading << endl;
    cout << "-----------------" << endl;
    if(static_cast<unsigned char>(command) < 5
       && static_cast<unsigned char>(command) > 0)
      {
	direction_commands(command, arg, x, y);
	return;
      }
   
    switch(command)
      {
      case(commands::forward): x+=arg*dx; y+=arg*dy; break;
      case(commands::left):
      case(commands::right):
	turn(command, arg);
      break;
      default: std::logic_error("couldn't do move command!");
      }
  };

  void direction_commands(const commands& command, int arg, int& xval, int& yval)
  {
    switch(command)
      {
      case(commands::north): yval+= arg; break;
      case(commands::east): xval+= arg; break;
      case(commands::south): yval-= arg; break;
      case(commands::west): xval-= arg; break;
      default:
	throw std::logic_error("invalid direction command:" + std::to_string((int) command));
      }

  };
  
  void do_command_waypoint(const commands& command, const int arg)
  {
    auto& [x,y] = position;
    auto& [wx, wy] = waypoint_position;

    if(static_cast<unsigned char>(command) < 5
       && static_cast<unsigned char>(command) > 0)
      {
	direction_commands(command, arg, wx, wy);
	return;
      }
    
    switch(command)
      {
      case(commands::forward) : x += arg*wx; y += arg* wy; break;
      case(commands::right):
      case(commands::left):
	rotate_waypoint(command, arg); break;
      default: throw std::logic_error("invalid direction command!");
      }
  }

  void rotate_waypoint(const commands& command, const int arg)
  {
    int n_right_angles = arg / 90;
    int sign = (static_cast<unsigned char>(command) == 5) ? 1 : -1;
    n_right_angles %= 4;
    n_right_angles *= sign;
    cout << "n_right_angles: "<< n_right_angles << endl;
    auto& [wx,wy] = waypoint_position;
    int newwx = fastcos(n_right_angles)*wx - fastsin(n_right_angles) * wy;
    int newwy = fastsin(n_right_angles)*wx + fastcos(n_right_angles)*wy;
    waypoint_position = {newwx, newwy};
  };
  
  
  void turn(const commands& command, const int arg)
  {
    int n_right_angles = arg / 90;
    auto curdir = static_cast<unsigned char>(currentheading) ;
    cout << "curdir: " << (int) curdir << endl;
    int sign = (static_cast<unsigned char>(command) == 5) ? -1 : 1;
    cout << "sign: " << sign << endl;
    int newdir = curdir + sign*n_right_angles;
    cout << "newdir: " << newdir << endl;
    newdir %= 4;
    if(newdir < 0)  newdir = 4+newdir;
    cout << "newdir: " << newdir << endl;
    currentheading = static_cast<facing>(newdir);
  }

  std::tuple<int,int> position = {0,0};
  facing currentheading = facing::east;
  std::tuple<int,int> waypoint_position = {10,1};
  
};

void lrstrip(string& s)
{
  s.erase(s.begin(), std::find_if(s.begin(), s.end(),[](unsigned char uc){return !std::isspace(uc);}));
  s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char uc){return !std::isspace(uc);}).base(), s.end());
}


template<typename In>
std::vector<std::tuple<commands, int>> parse_input(In& s)
{
  string line;
  std::vector<std::tuple<commands,int>> out;
  while(std::getline(s,line))
    {
      auto argstr = line.substr(1);
      lrstrip(argstr);
      auto arg = std::stoi(argstr);
      switch(line[0])
	{
	case('F'): out.emplace_back(commands::forward,arg) ; break;
	case('N'): out.emplace_back(commands::north, arg); break;
	case('E'): out.emplace_back(commands::east, arg); break;
	case('S'): out.emplace_back(commands::south, arg); break;
	case('W'): out.emplace_back(commands::west, arg); break;
	case('L'): out.emplace_back(commands::left, arg); break;
	case('R'): out.emplace_back(commands::right, arg); break;
	default: throw std::logic_error("failed to parse command");
	}
    }	 
  return out;
};

int manhattan_distance(const std::tuple<int,int>& pos)
{
  auto [x,y] = pos;
  return std::abs(x) + std::abs(y);
}

int main(int argc, char** argv)
{
  cout << "hello " << endl;
  std::ifstream ifs("input.txt");
  auto instructions = parse_input(ifs);

  ship s;
  for(auto& [c,a] : instructions)
    {
      cout << "command: " << (int) c << " arg: " << a << endl;
      s.do_command(c, a);
      auto[x,y] = s.position;
      cout << "x: " << x << "y: " << y << endl;
    }

  cout << "manhattan distance: " << manhattan_distance(s.position) << endl;

  ship s2;
  for(auto& [c,a] : instructions)
    {
      cout << "command: " << (int) c << " arg: " << a << endl;
      s2.do_command_waypoint(c, a);
      auto[x,y] = s2.position;
      cout << "x: " << x << " y: " << y << endl;
      auto[wx,wy]  = s2.waypoint_position;
      cout << "wx: " << wx << " wy: " << wy << endl;
    }

  cout << "manhattan distance: " << manhattan_distance(s2.position) << endl;

  for(int i=-4; i < 4; i++)
    cout << "cos(" << i << "): " << fastcos(i) <<endl;

  for(int i=-4; i< 4; i++)
    cout << "sin(" << i << "):" << fastsin(i) << endl;
  
}
