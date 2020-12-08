#include <iostream>
#include <string>
#include <vector>
#include <tuple>
#include <algorithm>
#include <fstream>
#include <set>

using std::vector;
using std::tuple;
using std::cout;
using std::endl;
using std::string;

enum class instructions {add, nop, jump};

class emulator
{
public:
  emulator(const vector<tuple<instructions, int>>& program)
    : _program(program) {};

  void update_proram(const vector<tuple<instructions,int>>& program)
  {
    _program = program;
  };
  
  void reset()
  {
    _accumulator = 0;
    _instruction_ptr = 0;
    _halted = false;
  }

  void do_nop(int arg) {_instruction_ptr++;};
  void do_acc(int arg) {_accumulator += arg; _instruction_ptr++;};
  void do_jmp(int arg) {_instruction_ptr += arg;};
  
  
  int instruction_ptr() const {return _instruction_ptr;};
  int accumulator() const {return _accumulator;};
  bool halted() const { return _halted;};

  void step()
  {
    if(_instruction_ptr >= _program.size())
      {
	cout << "instrution_ptr: " << _instruction_ptr << endl;
	cout << "program_size: " << _program.size() << endl;
	cout << "program halted" << endl;
	_halted = true;
	return;
      }
    auto [instr, arg] = _program[_instruction_ptr];
    cout << "arg: " << arg << endl;
    switch(instr)
      {
      case(instructions::add) : do_acc(arg); break;
      case(instructions::nop) : do_nop(arg); break;
      case(instructions::jump) : do_jmp(arg); break;
      }
  };
  
private:
  vector<tuple<instructions,int>> _program;
  int _accumulator = 0;
  int _instruction_ptr = 0;
  bool _halted = false;
};

void lrstrip(string& s)
{
  s.erase(s.begin(), std::find_if(s.begin(), s.end(),[](unsigned char uc){return !std::isspace(uc);}));
  s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char uc){return !std::isspace(uc);}).base(), s.end());
}

tuple<instructions, int> parse_instruction(const string& instr)
{
  auto pivot = instr.find(" ");
  instructions instout;
  switch(instr[0])
    {
    case('a'): instout = instructions::add; break;
    case('n'): instout = instructions::nop; break;
    case('j'): instout = instructions::jump; break;
    default:
      cout << "symbol: " << instr[0];
      throw std::runtime_error("invalid instruction symbol");
    };

  auto argstr = instr.substr(pivot+1);
  lrstrip(argstr);
  int arg = std::stoi(argstr);
  return std::make_tuple(instout, arg);
}


int find_acc_on_loop(emulator& emu)
{
  std::set<int> visited_locations;
  while(true)
    {
      cout <<"stepping...";
      auto acc_before = emu.accumulator();
      emu.step();
      auto pos = visited_locations.find(emu.instruction_ptr());
      if(pos != visited_locations.end())
	{
	  cout << "loop detected!" << endl;
	  return acc_before;
	};
      visited_locations.insert(emu.instruction_ptr());
    }
};
  
bool detect_program_loop(emulator& emu)
{
  std::set<int> visited_locations;
  while(true)
    {
      cout << "stepping..>" << endl;
      emu.step();
      cout << "checking if halted.." << endl;
      if(emu.halted())
	return false;
      auto iptr = emu.instruction_ptr();
      std::set<int>::const_iterator pos  = visited_locations.find(iptr);
      if(pos != visited_locations.cend())
	{
	  cout << "program looped! " << endl;
	  return true;
	}
      visited_locations.insert(iptr);
      cout << "visited location size: " << visited_locations.size() << endl;
    };

};


int main(int argc, char** argv)
{
  cout << "hello" << endl;
  std::ifstream ifs("input.txt");
  string line;
  vector<tuple<instructions, int>> program;
  while(std::getline(ifs, line))
    {
      program.push_back(parse_instruction(line));
    };

  cout << "there are: " << program.size() << " program lines"  << endl;

  emulator emu(std::move(program));

  auto val = find_acc_on_loop(emu);
  cout << "accumulator value before loop: " << val << endl;

  emu.reset();
  cout << std::boolalpha << detect_program_loop(emu) << endl;
  

  auto nopjmpiter = program.begin();
  while(nopjmpiter != program.end())
    {
      nopjmpiter = std::find_if(nopjmpiter, program.end(),
				[](auto& item)
				{ auto [instr, arg] = item;
				  return (instr == instructions::jump)
				  || (instr == instructions::nop);});

      auto distance = std::distance(program.begin(), nopjmpiter);
      nopjmpiter++;
      cout << "found changeable instruction at position: " << distance << endl;
      vector<tuple<instructions, int>> progcopy(program.begin(), program.end());
      auto [instr, arg] = progcopy[distance];
      instr = (instr == instructions::nop) ? instructions::jump : instructions::nop;
      progcopy[distance] = std::make_tuple(instr, arg);

      cout << "copy size:  " << progcopy.size() << endl;
      emu.update_proram(progcopy);
      cout << "resetting"<< endl;
      emu.reset();
      cout << "reset donw.. " << endl;
      auto loops = detect_program_loop(emu);
      cout << "out of function" << endl;
      if(not loops)
	{
  cout << "found a program that doesn't loop!" << endl;
	  cout << "accumulator value: " << emu.accumulator() << endl;
	  break;
	}
    }
};
