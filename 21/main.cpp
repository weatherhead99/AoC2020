#include <iostream>
#include <string>
#include <string_view>
#include <fstream>
#include <unordered_set>
#include <tuple>
#include <algorithm>
#include <vector>
#include <unordered_map>


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
      auto substr = subline.substr(spcpos+1);
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
  std::ifstream ifs("input2.txt");
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

  std::unordered_map<string, std::vector<string>> possible_food_allergens_map;
  for(auto& a : allallergens)
    {
      std::unordered_set<string> possible_foods;
      auto foodit = foodsvec.begin();
      for(auto& ca : allergensvec)
	{
	  auto pos = std::find(ca.cbegin(), ca.cend(), a);
	  if(pos != ca.end())
	    {
	      for(auto& f : *(foodit++))
		{
		  possible_foods.insert(f);
		}
	    };
	}
      std::vector<string> outvec(possible_foods.begin(), possible_foods.end());
      std::sort(outvec.begin(), outvec.end());
      possible_food_allergens_map[a] = outvec;
    }
  
  for(auto& [k,v] : possible_food_allergens_map)
    {
      cout << "allergen: " << k << endl;
      cout << "----" << endl;
      for(auto& a : v)
	{
	  cout << a << ", ";
	}
      cout << endl;
    }
  
}
