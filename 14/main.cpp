#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include <numeric>
#include <bitset>
#include <tuple>
#include <map>
#include <vector>
#include <set>

using std::cout;
using std::endl;
using std::string;

using U = unsigned long long;

struct mem_command
{
  U maskvals = 0;
  U maskbits = 0;
  std::vector<U> addrs;
  std::vector<U> writevals;

  void clear()
  {
    addrs.clear();
    writevals.clear();
    maskvals = 0;
    maskbits = 0;
  }
};

U apply_mask(U val, U maskbits, U maskval)
{
  std::bitset<36> maskbitset(maskbits);
  std::bitset<36> maskvalset(maskval);
  std::bitset<36> out(val);

  for(int i=0; i < 36; i++)
    {
      if(maskbitset[i])
	out[i] = maskvalset[i];
    }

  return out.to_ullong();
}

U apply_mask_part2(U val, U maskval)
{
  std::bitset<36> out(val);
  std::bitset<36> maskvalset(maskval);
  for(int i=0; i < 36; i++)
    {
      if(maskvalset[i])
	out[i] = 1;
    }
  return out.to_ullong();
}

U get_X_bits(U maskbits, U maskvals)
{
  std::bitset<36> maskbitset(maskbits);
  std::bitset<36> maskvalset(maskvals);

  std::bitset<36> out(0);
  for(int i=0; i < 36; i++)
    {
      if(maskbitset[i] == false && maskvalset[i] == false)
	out[i] = true;
    }
  return out.to_ullong();
}


std::set<U> enumerate_possibilities(U addr, U Xbits, U maskval)
{
  std::bitset<36> maskbitset(Xbits);
  //find maximum set mask bit
  U possibilities = 1 << maskbitset.count();
  std::set<U> out;
  out.insert(apply_mask_part2(addr, maskval));
  
  for(int i=0; i < 36; i++)
      if(maskbitset[i])
	  for(auto& v : out)
	    {
	      std::bitset<36> newval(v);
	      newval.flip(i);
	      out.insert(newval.to_ullong());
	    }
  return out;
};


struct mem
{
  std::map<U, U> _memmap;

  void apply_command(const mem_command& com)
  {
    auto addrit = com.addrs.begin();
    for(auto& val : com.writevals)
      _memmap[*addrit++] = apply_mask(val, com.maskbits, com.maskvals);
  };

  void apply_command_part2(const mem_command& com)
  {
    auto addrit = com.addrs.begin();
    for(auto& val : com.writevals)
      {
	auto xbits = get_X_bits(com.maskbits, com.maskvals);
	auto addr_possibles = enumerate_possibilities(*addrit++, xbits, com.maskvals);
	for(auto& possible: addr_possibles)
	  {
	    _memmap[possible] = val;
	  }
      }
  };

  U memory_sum() const
  {
    return std::accumulate(_memmap.begin(), _memmap.end(),
			   0ull, [](auto& acc, auto& v)
			   { return acc + v.second;});
  }
};


void lrstrip(std::string& s)
{
  s.erase(s.begin(), std::find_if(s.begin(), s.end(),[](unsigned char uc){return !std::isspace(uc);}));
  s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char uc){return !std::isspace(uc);}).base(), s.end());
}

std::tuple<U, U> parse_mask_string(const string& s)
{
  std::bitset<36> bs_mask(0);
  std::bitset<36> bs_vals(0);
  for(int i=0; i < 36; i++)
    {
      if(s[i] == 'X')
	{
	  bs_mask.set(35- i, false);
	  bs_vals.set(35-i, false);
	}
      else
	{
	  bs_mask.set(35-i, true);
	  bs_vals.set(35-i, s[i] - (int) '0');
	}
    }
  return {bs_mask.to_ullong(), bs_vals.to_ullong()};
};




template<typename Stream>
std::vector<mem_command> parse_input(Stream&& strm)
{
  bool first = true;
  string line;
  std::vector<mem_command> out;
  mem_command current_command;
  while(std::getline(strm, line))
    {
      auto delimpos = line.find('=');
      auto cmd = line.substr(0, delimpos);
      lrstrip(cmd);
      cout << "cmd: " << cmd << endl;
      if(cmd == "mask")
	{
	  if(not first)
	    {
	      cout << "pushing back command" << endl;;
	      out.push_back(current_command);
	      current_command.clear();
	    }
	  
	  auto mskstr = line.substr(delimpos +1);
	  lrstrip(mskstr);
	  cout << mskstr << endl;
	  cout << mskstr.size() << endl;
	  auto [mskbits, mskvals] = parse_mask_string(mskstr);
	  current_command.maskbits = mskbits;
	  current_command.maskvals = mskvals;
	  first = false;
	}
      else
	{
	  auto leftdelim = line.find('[');
	  auto rightdelim = line.find(']');
	  auto memaddrstr = line.substr(leftdelim+1, rightdelim-leftdelim-1);
	  auto memaddr = std::stoull(memaddrstr);
	  auto memvalstr = line.substr(delimpos+1);
	  cout << "length of memval: " << memvalstr.size() << endl;
	  cout << "memvalstr: " << memvalstr << endl;
	  lrstrip(memvalstr);
	  auto memval = std::stoull(memvalstr);
	  current_command.addrs.push_back(memaddr);
	  current_command.writevals.push_back(memval);
	};

    }
  out.push_back(current_command);
  return out;
};



int main(int argc, char** argv)
{
  std::ifstream ifs("input.txt");
  auto cmds = parse_input(ifs);

  cout << "number of commands: " << cmds.size() << endl;
  mem m;

  for(auto& com : cmds)
    m.apply_command(com);

   for(auto& [k,v] : m._memmap)
     cout << "k: " << k << ", v: " << std::bitset<36>(v) << endl;

  cout << "memory sum: " << m.memory_sum() << endl;

  mem m2;
  for(auto& com : cmds)
    m2.apply_command_part2(com);

  cout << "memory sum part 2: " << m2.memory_sum() << endl;
};
