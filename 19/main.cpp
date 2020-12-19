#include <iostream>
#include <string>
#include <map>
#include <variant>
#include <tuple>
#include <vector>
#include <fstream>
#include <sstream>
#include <deque>

using std::cout;
using std::endl;
using std::string;


using rule = std::variant<char, std::vector<std::vector<int>>>;

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
	  //cout << "rulenum: " << rulenum << " has quotes" << endl;
	  out[rulenum] = line[quotepos1+1];
	}
      else if(alternativepos != string::npos)
	{
	  //	  cout << "rulenum: " << rulenum << "has alternates" <<  endl;
	  std::vector<std::vector<int>> alts;
	  alts.reserve(2);
	  std::vector<int> alt1;
	  alt1.resize(2);
	  std::istringstream iss(line.substr(delimpos+1, alternativepos - (delimpos+1)));
	  iss >> alt1[0] >> alt1[1];
	  alts.push_back(alt1);
	  iss.str(line.substr(alternativepos +1));
	  std::vector<int> alt2;
	  alt2.resize(2);
	  iss >> alt2[0] >> alt2[1];
	  alts.push_back(alt2);
	  out[rulenum] = alts; 
	}
      else
	{
	  //	  cout << "rulenum: " << rulenum << " has no alternatives" << endl;
	  std::vector<std::vector<int>> alts;
	  alts.reserve(1);
	  std::vector<int> alt1;
	  alt1.resize(2);
	  std::istringstream iss(line.substr(delimpos+1));
	  iss >> alt1[0] >> alt1[1];
	  alts.push_back(alt1);
	  out[rulenum] = alts;
	  
	}
    }

  return out;
};





bool match_rule(const rule& r,  const std::map<int, rule>& rulesmap,
		std::istringstream& iss, int level = 0)
{
  cout << "tellg: " << iss.tellg() << endl;
  if(const char* matchchar = std::get_if<char>(&r))
    {
      //cout << "char rule" << endl;
      if(char nextchar = iss.get())
	{
	  cout << "nextchar:"  << nextchar << "matchchar: " << *matchchar << endl;
	  if(nextchar == *matchchar)
	    {
	      //cout << "matched!" << endl;
	      if(level == 0)
		iss.putback(nextchar);
	      //cout << "char match" << endl;
	      return true;
	    }
	  iss.putback(nextchar);
	  return false;
	}
      else
	{
	  //all characters matched
	  return true;
	}
      throw std::logic_error("char rule escaped block!");
    }
  else
    {
      auto& subrules = std::get<std::vector<std::vector<int>>>(r);
      for(auto& alternative_ruleset : subrules)
	{
	  bool matches_inner = true;
	  std::deque<char> taken_chars;
	  for(auto& ruleidx: alternative_ruleset)
	    {
	      cout << "ruleidx: " << ruleidx << endl;
	      const auto& rule = rulesmap.at(ruleidx);
	      bool thismatch = match_rule(rule, rulesmap, iss, level+1);
	      char taken  = iss.get();
	      taken_chars.push_back(taken);
	      matches_inner &= thismatch;
	      
	    };
	  if(matches_inner)
	    {
	      return true;
	    }
	  else
	    {
	      while(!taken_chars.empty())
		{
		  iss.putback(taken_chars.front());
		  taken_chars.pop_front();
		}
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
  return match;
}



int main(int argc, char** argv)
{
  std::ifstream ifs("input2.txt");
  auto rules = parse_rules(ifs);
  string line;
  auto rule0 = rules.at(0);
  while(std::getline(ifs, line))
    {
      cout << "considering line: " << line << endl;
      auto matches = match_rule(rule0, rules, line);
      cout << "matches rule 0? " << std::boolalpha << matches << endl;
    }
  

}
