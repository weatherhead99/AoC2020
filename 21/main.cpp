#include <iostream>
#include <string>
#include <string_view>
#include <fstream>
#include <unordered_set>
#include <tuple>
#include <algorithm>
#include <vector>
#include <unordered_map>
#include <sstream>

using std::cout;
using std::endl;
using std::string;

using ingredients = std::unordered_set<string>;

void lrstrip(std::string& s)
{
  s.erase(s.begin(), std::find_if(s.begin(), s.end(),[](unsigned char uc){return !std::isspace(uc);}));
  s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char uc){return !std::isspace(uc);}).base(), s.end());
}

std::vector<string> parse_set(const string& subline, const string& delim, bool extra=false)
{
  std::vector<string> out;
  auto start = 0;
  auto spcpos = subline.find(delim);
  while(spcpos != string::npos)
    {
      auto substr = subline.substr(start, spcpos - start);
      lrstrip(substr);
      out.push_back(substr);
      start = spcpos + 1;
      spcpos = subline.find(delim, start);
    }
  if(extra)
    {
      auto substr = subline.substr(start);
      lrstrip(substr);
      out.push_back(substr);
    }

  std::sort(out.begin(), out.end());
  return out;
}


std::tuple<std::vector<string>, std::vector<string>> parse_ingredient_line(const string& line)
{
  auto parenstartpos = line.find("(");
  auto parenendpos = line.find(")");
  auto containspos = line.find("contains ");

  auto foodnames = line.substr(0,parenstartpos);
  
  auto foods = parse_set(foodnames, " ");

  auto allergenspcpos = line.find(" ",containspos);
  auto allergennames = line.substr(allergenspcpos+1, parenendpos - allergenspcpos -1);
  
  auto allergens = parse_set(allergennames, ",", true);
  return {foods, allergens};
}


int main(int argc, char** argv)
{
  std::ifstream ifs("input.txt");
  string line;
  std::vector<std::vector<string>> foodsvec;
  std::vector<std::vector<string>> allergensvec;
  while(std::getline(ifs, line))
    {
      auto [foods, allergens] = parse_ingredient_line(line);
      foodsvec.push_back(foods);
      allergensvec.push_back(allergens);
    };

  ingredients allallergens;
  for(auto& all : allergensvec)
    {
      for(auto& a : all)
	allallergens.insert(a);
    }  
  cout << "total allergens: " << allallergens.size() << endl;

  for(auto& a : allallergens)
    {
      cout << a << ",";
    }
  cout << endl;

  ingredients allfoods;
  for(auto& all : foodsvec)
    {
      for(auto& a : all)
	allfoods.insert(a);
    }

  
  std::unordered_map<string, std::vector<string>> possible_foods_map;
  
  for(auto& a : allallergens)
    {
      std::vector<string> possible_foods;
      auto foodit = foodsvec.begin();
      for(auto& ca : allergensvec)
	{
	  auto pos = std::find(ca.begin(), ca.end(), a);
	  if(pos != ca.end())
	    {
	      if(possible_foods.empty())
		{
		  possible_foods.resize(foodit->size());
		  std::copy(foodit->begin(), foodit->end(), possible_foods.begin());
		}
	      else
		{
		  std::vector<string> temp;
		  temp.reserve(possible_foods.size());
		  std::sort(possible_foods.begin(), possible_foods.end());
		  std::set_intersection(possible_foods.begin(), possible_foods.end(),
					foodit->begin(), foodit->end(), std::back_inserter(temp));
		  possible_foods = temp;
		}
	    }
	  foodit++;
	}
      possible_foods_map[a] = possible_foods;
    }


  std::unordered_set<string> all_allergenic_foods;
  for(auto& [k,v] : possible_foods_map)
    {
      all_allergenic_foods.insert(v.begin(), v.end());
    };
  

  std::vector<string> unallergenic_foods;
  for(auto& food: allfoods)
    {
      if(all_allergenic_foods.find(food) != all_allergenic_foods.end())
	continue;
      unallergenic_foods.push_back(food);
    };

  
  for(auto& fd: unallergenic_foods)
    cout << fd << ",";
  cout << endl;

  //count number of occurrences;
  int total_count = 0;
  for(auto& fd:  unallergenic_foods)
    {
      for(auto& foodlist: foodsvec)
	{
	  total_count += std::count(foodlist.begin(), foodlist.end(), fd);
	};
    }

  cout << "total unallergenic food appearance: " << total_count << endl;

  while(true)
    {
      bool any_length_gt_1 = false;

      for(auto& [k,v] : possible_foods_map)
	{
	  if(v.size() > 1)
	    {
	      cout << "key: " << k << "gr length 1" << endl;
	      any_length_gt_1 = true;
	    }
	  else
	    {
	      cout << "allergen: " << k << ", food:" << v.front() << endl;
	      for(auto& [k2, v2] : possible_foods_map)
		{
		  if(k2 ==k)
		    continue;
		  auto pos = std::find(v2.begin(), v2.end(), v.front());
		  if(pos != v2.end())
		    v2.erase(pos);
		 
		}
	    }

	};
      
      if(!any_length_gt_1)
	break;
      
    }


  

  for(auto& [k,v] : possible_foods_map)
    {
      cout << "allergen:" << k << endl;
      for(auto& a : v)
	cout << a << ",";
      cout << endl;
    }

  std::vector<string> sorted_allergen_list;
  for(auto&[k,v] : possible_foods_map)
    sorted_allergen_list.push_back(k);
  std::sort(sorted_allergen_list.begin(), sorted_allergen_list.end());

  std::ostringstream oss;
  std::ostringstream oss2;
  for(auto& sa: sorted_allergen_list)
    {
      auto food = possible_foods_map.at(sa).front();
      oss << food << ",";
      oss2 << sa << ",";
    }

  cout << "canonical list: " << oss.str() << endl;
  cout << "allergen list: " << oss2.str() << endl;
}
