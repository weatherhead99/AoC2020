#include <iostream>
#include <vector>
#include <string>
#include <tuple>
#include <algorithm>
#include <unordered_map>
#include <optional>
#include <list>
#include <fstream>
#include <unordered_set>
#include <numeric>

using std::string;
using std::vector;

using std::cout;
using std::endl;


void lrstrip(string& s)
{
  s.erase(s.begin(), std::find_if(s.begin(), s.end(),[](unsigned char uc){return !std::isspace(uc);}));
  s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char uc){return !std::isspace(uc);}).base(), s.end());
}

string get_bag_colour(const string& s)
{
  auto piter = s.find("bag");
  auto colour = s.substr(0, piter);
  lrstrip(colour);
  return colour;
};

using bagtup = std::tuple<string, vector<string>, vector<int>>;


bagtup parse_line(const string& s)
{
  auto piter = s.find("contain") + 7;
  string container = get_bag_colour(s.substr(0,piter));
  vector<string> contained;
  vector<int> counts;

  auto allbags = s.substr(piter+1);
  auto sep = allbags.find_first_of(".,");
  auto lastsep = 0;
  do
    {
      auto bagstr = allbags.substr(lastsep, sep);
      lrstrip(bagstr);
      auto spcpos = bagstr.find(" ");
      auto numstr = bagstr.substr(0, spcpos);
      if(numstr != "no")
	{
	  counts.push_back(std::stoi(numstr));
	  contained.push_back(get_bag_colour(bagstr.substr(spcpos)));
	}
      
      lastsep = sep+1;
      sep = allbags.find_first_of(".,",lastsep+1);
    }
  while(sep != string::npos);
  return std::make_tuple(container, contained, counts);
};

using bagmap = std::unordered_map<string, std::tuple<vector<string>,vector<int>>>;

bagmap build_bag_map(const std::vector<bagtup>& inputs)
{
  bagmap out;
  for(auto& i : inputs)
    {
      auto container = std::get<0>(i);
      auto contained = std::get<1>(i);
      auto counts = std::get<2>(i);
      out[container] = std::make_tuple(contained,counts);
    }

  return out;
};


std::unordered_set<string> find_possible_containers(const bagmap& map, const string& contained)
{
  std::unordered_set<string> possible_keys;
  //cout << "searching for: "<< contained << endl;
  for(auto& [k,v]: map)
    {
      auto colours = std::get<0>(v);
      auto pos = std::find(colours.cbegin(), colours.cend(), contained);
      if(pos != colours.cend())
	{
	  possible_keys.insert(k);
	  auto recurse_keys = find_possible_containers(map,k);
	  possible_keys.insert(recurse_keys.begin(), recurse_keys.end());
	  //cout << "--key: " << k << endl;
	}     
    };
  return possible_keys;
};

int count_total_bags(const bagmap& map, const string& colour, bool top_level=true)
{
  auto v = map.find(colour);
  
  auto contained_colours = std::get<0>(v->second);
  auto counts = std::get<1>(v->second);

  //  cout << "colour: " << colour;
  if(contained_colours.size() == 0)
    {
      return 1;
    };

  vector<int> recurse_counts;
  recurse_counts.reserve(contained_colours.size());
  std::transform(contained_colours.begin(), contained_colours.end(),
		 recurse_counts.begin(),
		 [&map](const string& s) { return count_total_bags(map, s,false);});

  auto total = std::inner_product(counts.begin(), counts.end(),
					  recurse_counts.begin(),  0);
  if(!top_level)
    total++;

  //cout << "total: " << total;
  return total;
}




int main(int argc, char** argv)
{
  std::ifstream ifs("input.txt");
  string line;
  std::vector<bagtup> bags;
  while(std::getline(ifs, line))
    {
      auto bt = parse_line(line);
      bags.push_back(bt);
    };

  cout << "number of bag lines:" << bags.size() << endl;
  string target_colour = "shiny gold";
  auto bm = build_bag_map(bags);
  auto poskeys = find_possible_containers(bm, target_colour);
  cout << "possible keys: " ;
  for(auto& k : poskeys)
    {
      cout << k << ", ";
    }
  cout << endl;
  
  cout << "number of possible keys: " << poskeys.size() << endl;
  auto total_bags = count_total_bags(bm, target_colour);
  cout << "total number of bags: " << total_bags << endl;
}
  
