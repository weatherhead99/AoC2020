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





bool match_rule(const rule& r,  const std::map<int, rule>& rulesmap,
		std::istringstream& iss, int level = 0)
{
  //cout << "tellg: " << iss.tellg() << endl;
  if(const char* matchchar = std::get_if<char>(&r))
    {
      //cout << "char rule" << endl;
      char nextchar;
      if(iss.get(nextchar))
	{
	  //cout << "nextchar:"  << nextchar << "matchchar: " << *matchchar << endl;
	  if(nextchar == *matchchar)
	    {
	      //cout << "char match" << endl;
	      return true;
	    }
	  iss.putback(nextchar);
	  return false;
	}
      else
	{
	  //cout << "all characters matched" << endl;
	  //all characters matched
	  return true;
	}
      throw std::logic_error("char rule escaped block!");
    }
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



int main(int argc, char** argv)
{
  std::ifstream ifs("input.txt");
  auto rules = parse_rules(ifs);
  string line;
  auto rule0 = rules.at(0);
  int matchcount = 0;

  
  //part 1
  while(std::getline(ifs, line))
    {
      cout << "considering line: " << line << endl;
      auto matches = match_rule(rule0, rules, line);
      if(matches)
  	matchcount++;
      cout << "matches rule 0? " << std::boolalpha << matches << endl;
    }
  cout << "matchcount: " << matchcount << endl;

}
