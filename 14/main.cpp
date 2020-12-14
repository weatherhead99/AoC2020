#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include <numeric>
#include <bitset>
#include <tuple>
#include <map>

using std::cout;
using std::endl;
using std::string;

struct mem_command
{
  unsigned long maskvals = 0;
  unsigned long maskbits = 0;
  std::vector<unsigned long> addrs;
  std::vector<unsigned long> writevals;

  void clear()
  {
    addrs.clear();
    writevals.clear();
    maskvals = 0;
    maskbits = 0;
  }
};

unsigned long apply_mask(unsigned long val,
			 unsigned long maskbits,
			 unsigned long maskval)
{
  std::bitset<36> maskbitset(maskbits);
  std::bitset<36> maskvalset(maskval);

  std::bitset<36> out(maskval);

  cout << "out: " << out.to_ulong() << endl;
  for(int i=0; i < 36; i++)
    {
      if(maskbitset[i])
	out[i] = maskvalset[i];
    }

  cout << "out: " << out.to_ulong() << endl;
  return out.to_ulong();
}


struct mem
{
  std::map<unsigned long, unsigned long> _memmap;
  void set_mem_val(unsigned long addr, unsigned long val,
		   unsigned long maskbits, unsigned long maskval)
  {
    cout << "val: " << val << endl;
    cout << "maskbits: "<< maskbits << endl;
    cout << "maskval: "<< maskval << endl;
    _memmap[addr] = apply_mask(val, maskbits, maskval);
  };

  void apply_command(const mem_command& com)
  {
    auto addrit = com.addrs.begin();
    for(auto& val : com.writevals)
      set_mem_val(*addrit++, val, com.maskbits, com.maskvals);
  };
 
};


void lrstrip(std::string& s)
{
  s.erase(s.begin(), std::find_if(s.begin(), s.end(),[](unsigned char uc){return !std::isspace(uc);}));
  s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char uc){return !std::isspace(uc);}).base(), s.end());
}

std::tuple<unsigned long, unsigned long> parse_mask_string(const string& s)
{
  std::bitset<36> bs_mask(0);
  std::bitset<36> bs_vals(0);
  for(int i=0; i < 36; i++)
    {
      if(s[i] == 'X')
	{
	  bs_mask.set(i, false);
	  bs_vals.set(i, false);
	}
      else
	{
	  bs_mask.set(i, true);
	  bs_vals.set(i, s[i] - (int) '0');
	}
    }
  cout << "bs_mask: " << bs_mask.to_ulong() << endl;
  cout << "bs_vals: " << bs_vals.to_ulong() << endl;
  return {bs_mask.to_ulong(), bs_vals.to_ulong()};
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
      
      if(cmd == "mask")
	{
	  if(not first)
	    {
	      cout << "pushing back command";
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
	  cout << "addrstr: "<< memaddrstr << endl;
	  auto memaddr = std::stoul(memaddrstr);
	  cout << "addr: " << memaddr << endl;
	  auto memvalstr = line.substr(delimpos+1);
	  lrstrip(memvalstr);
	  auto memval = std::stoul(memvalstr);
	  current_command.addrs.push_back(memaddr);
	  current_command.writevals.push_back(memval);
	};

    }
  out.push_back(current_command);
  return out;
};



int main(int argc, char** argv)
{
  std::ifstream ifs("input2.txt");
  auto cmds = parse_input(ifs);

  cout << "number of commands: " << cmds.size() << endl;
  mem m;

  for(auto& com : cmds)
    m.apply_command(com);
  
  auto sm = std::accumulate(m._memmap.begin(), m._memmap.end(),
			    0, [](auto& acc, auto& v)
			    { return acc + v.second;});

  cout << "memory sum: " << sm << endl;

  for(auto& [k,v] : m._memmap)
    cout << "k: " << k << ", v: " << v << endl;

};
