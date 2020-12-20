#include <iostream>
#include <string>
#include <map>
#include <variant>
#include <tuple>
#include <vector>
#include <fstream>
#include <sstream>
#include <deque>
#include <algorithm>

using std::cout;
using std::endl;
using std::string;


using rule = std::variant<char, std::vector<std::vector<int>>>;

std::vector<int> parse_rule_string(const string& in)
{
  std::istringstream iss(in);
  std::vector<int> out;
  int val = 0;
  while(iss >> val || !iss.eof())
    {
      if(!iss.fail())
	out.push_back(val);
    }
  
  return out;
}


std::map<int, rule> parse_rules(std::ifstream& ifs)
{
  string line;
  std::map<int, rule> out;
  while(std::getline(ifs, line))
    {
      if(line.size() == 0)
	{
	  break;
	}
      
      auto delimpos = line.find(':');
      auto rulenum = std::stoi(line.substr(0, delimpos));

      //find plain char rules
      auto quotepos1 = line.find("\"");
      auto alternativepos = line.find("|");
      if(quotepos1 != string::npos)
	{
	  out[rulenum] = static_cast<char>(line[quotepos1+1]);
	}
      else if(alternativepos != string::npos)
	{
	  //	  cout << "rulenum: " << rulenum << "has alternates" <<  endl;
	  std::vector<std::vector<int>> alts;
	  alts.reserve(2);
	  alts.push_back(parse_rule_string(line.substr(delimpos+1, alternativepos-(delimpos+1))));
	  alts.push_back(parse_rule_string(line.substr(alternativepos+1)));
	  out[rulenum] = alts; 
	}
      else
	{
	  //	  cout << "rulenum: " << rulenum << " has no alternatives" << endl;
	  std::vector<std::vector<int>> alts;
	  alts.reserve(1);
	  alts.push_back(parse_rule_string(line.substr(delimpos+1)));
	  out[rulenum] = alts;
	  
	}
    }

  return out;
};


bool char_match(char matchchar, std::istringstream& iss)
{
  char nextchar;
  if(iss.get(nextchar))
    {
      if(nextchar == matchchar)
	return true;
      iss.putback(nextchar);
      return false;
    }
  else
    return true;
  throw std::logic_error("char rule escaped block!");

}



bool match_rule(const rule& r,  const std::map<int, rule>& rulesmap,
		std::istringstream& iss, int level = 0)
{
  if(const char* matchchar = std::get_if<char>(&r))
      return char_match(*matchchar, iss);
  else
    {
      auto& subrules = std::get<std::vector<std::vector<int>>>(r);
      auto init_seek = iss.tellg();
      for(auto& alternative_ruleset : subrules)
	{
	  bool matches_inner = true;
	  for(auto& ruleidx: alternative_ruleset)
	    {
	      //cout <<  ruleidx << ",";
	      if(ruleidx ==0)
		{
		throw std::logic_error("called rule 0!");
		}
	      
	      const auto& rule = rulesmap.at(ruleidx);
	      bool thismatch = match_rule(rule, rulesmap, iss, level+1);
	      matches_inner &= thismatch;
	      
	    };
	  //cout << endl;
	  if(matches_inner)
	    {
	      //cout << "inner match: " << endl;
	      return true;
	    }
	  else
	    {
	      iss.seekg(init_seek);
	    }
	};
      
      //cout << "no ruleset matched, returning false" << endl;
      return false;
    }

};

bool match_rule(const rule& r, const std::map<int, rule>& rulesmap,
		const string& in)
{
  std::istringstream iss(in);
    bool match = match_rule(r, rulesmap, iss,0);

  cout << "tellg: "<< iss.tellg() << endl;
  if(match && ( (iss.tellg() != -1  && iss.tellg() != in.size())))
    {
      cout << "matches, but rejected due to tellg() constraint" << endl;
      return false;
    }
  return match;
}

bool match_any_number_of_times(std::istringstream& iss, const rule& r, const std::map<int,rule>& rulesmap, int level)
{
  bool anymatch = false;
  while(!iss.fail())
    {
      auto init_seek = iss.tellg();
      auto match = match_rule(r, rulesmap, iss, level+1);
      
      anymatch |= match;
      if(!match)
	{
	  iss.seekg(init_seek);
	  break;
	}
    }
  //cout << endl;
  
  return anymatch;
}



bool special_rule_8(std::istringstream& iss, const std::map<int, rule>& rulesmap, int level)
{
  auto rule_42 = rulesmap.at(42);
  cout << "special rule 8 invoked " << endl;
  return match_any_number_of_times(iss, rule_42, rulesmap, level);
};


bool special_rule_11(std::istringstream& iss, const std::map<int,rule>& rulesmap, int level)
{
  cout << "special rule 11 invoked" << endl;
  auto rule_42 = rulesmap.at(42);
  auto rule_31 = rulesmap.at(31);
  auto init_seek = iss.tellg();
  bool match_42 = match_any_number_of_times(iss, rule_42, rulesmap, level);
  bool match_31 = match_any_number_of_times(iss, rule_31, rulesmap, level);

  bool bothmatch = match_42 && match_31;
  if(!bothmatch)
    iss.seekg(init_seek);
  
  return bothmatch;
};


bool match_rule_part2(const rule& r,  const std::map<int, rule>& rulesmap,
		std::istringstream& iss, int level = 0)
{
  if(const char* matchchar = std::get_if<char>(&r))
      return char_match(*matchchar, iss);
  else
    {
      auto& subrules = std::get<std::vector<std::vector<int>>>(r);
      auto init_seek = iss.tellg();
      for(auto& alternative_ruleset : subrules)
	{
	  bool matches_inner = true;
	  for(auto& ruleidx: alternative_ruleset)
	    {
	      bool thismatch = false;
	      cout <<  ruleidx << ",";
	      if(ruleidx ==0)
		throw std::logic_error("called rule 0!");
	      else if(ruleidx ==8)
		matches_inner &= special_rule_8(iss, rulesmap, level);
	      else if(ruleidx ==11)
		matches_inner &= special_rule_11(iss, rulesmap, level);
	      else
		{
		  const auto& rule = rulesmap.at(ruleidx);
		  thismatch = match_rule_part2(rule, rulesmap, iss, level+1);
		  matches_inner &= thismatch;
		}
	      
	    };
	  //cout << endl;
	  if(matches_inner)
	    return true;
	  else
	    iss.seekg(init_seek);
	};
      
      //cout << "no ruleset matched, returning false" << endl;
      return false;
    }

};

bool match_rule_part2(const rule& r, const std::map<int, rule>& rulesmap,
		const string& in)
{
  std::istringstream iss(in);
  bool match = match_rule_part2(r, rulesmap, iss,0);

  cout << "tellg: "<< iss.tellg() << endl;
  if(match && ( (iss.tellg() != -1  && iss.tellg() != in.size())))
    {
      cout << "matches, but rejected due to tellg() constraint" << endl;
      return false;
    }
  return match;
}


int main(int argc, char** argv)
{
  std::ifstream ifs("input.txt");
  auto rules = parse_rules(ifs);
  string line;
  auto rule0 = rules.at(0);
  int matchcount = 0;

  std::vector<string> to_match;
  while(std::getline(ifs,line))
    to_match.push_back(line);
  
  //part 1
  for(auto& ln : to_match)
    {
      cout << "considering line: " << ln << endl;
      auto matches = match_rule(rule0, rules, ln);
      if(matches)
  	matchcount++;
      cout << "matches rule 0? " << std::boolalpha << matches << endl;
    }
  cout << "matchcount: " << matchcount << endl;

  cout << "---------------------------" << endl;
  //part2
  matchcount = 0;
  for(auto& ln : to_match)
    {
      cout << "considering line: " << ln << endl;
      auto matches = match_rule_part2(rule0, rules, ln);
      if(matches)
	matchcount++;
      cout << "matches rule 0? " << std::boolalpha << matches << endl;
    }
  cout << "matchcount: " << matchcount << endl;
}
