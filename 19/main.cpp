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
      if(rulenum == 69)
	cout << "69: rule line: " << line << endl;
      

      //find plain char rules
      auto quotepos1 = line.find("\"");
      auto alternativepos = line.find("|");
      if(quotepos1 != string::npos)
	{
	  if(rulenum ==69)
	    cout << "quotepos1: " << quotepos1 << endl;
	  //cout << "rulenum: " << rulenum << " has quotes" << endl;
	  cout << "chr: " << line[quotepos1+1] << endl;
	  out[rulenum] = static_cast<char>(line[quotepos1+1]);
	}
      else if(alternativepos != string::npos)
	{
	  //	  cout << "rulenum: " << rulenum << "has alternates" <<  endl;
	  std::vector<std::vector<int>> alts;
	  alts.reserve(2);
	  std::vector<int> alt1;
	  alt1.resize(2);
	  std::istringstream iss(line.substr(delimpos+1, alternativepos - (delimpos+1)));
	  if(rulenum ==20)
	    cout << "alternate1str: " << line.substr(delimpos+1, alternativepos - (delimpos+1)) << endl;
	  iss >> alt1[0] >> alt1[1];
	  alts.push_back(alt1);
	  iss.str(line.substr(alternativepos +1));
	  if(rulenum == 20)
	    cout << "alternate2str: " << line.substr(alternativepos+1) << endl;
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
	  //HACK to make it work, nasty
	  if(alt1[1] ==0)
	    alt1.resize(1);
	  
	  alts.push_back(alt1);
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
      cout << "char rule" << endl;
      if(char nextchar = iss.get())
	{
	  //ut << "nextchar:"  << nextchar << "matchchar: " << *matchchar << endl;
	  if(nextchar == *matchchar)
	    {
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
	  int matchcount = 0;
	  for(auto& ruleidx: alternative_ruleset)
	    {
	      cout << "ruleidx: " << ruleidx << endl;
	      if(ruleidx ==0)
		{
		throw std::logic_error("called rule 0!");
		}
	      
	      //cout << "ruleidx: " << ruleidx << endl;
	      const auto& rule = rulesmap.at(ruleidx);
	      bool thismatch = match_rule(rule, rulesmap, iss, level+1);
	      if(thismatch)
		matchcount++;
	      matches_inner &= thismatch;
	      
	    };
	  if(matches_inner)
	    {
	      return true;
	    }
	  else
	    {
	      for(int i=0 ; i < matchcount; i++)
		iss.unget();
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
  if(match && (iss.tellg() != in.size()))
    return false;
  return match;
}



int main(int argc, char** argv)
{
  std::ifstream ifs("input.txt");
  auto rules = parse_rules(ifs);
  string line;
  auto rule0 = rules.at(0);
  int matchcount = 0;

  for(auto& [k,v]: rules)
    {
      auto* subrules = std::get_if<std::vector<std::vector<int>>>(&v);
      if(subrules)
	{
	  for(auto& rule_alts : *subrules)
	    {
	      auto pos = std::find(rule_alts.begin(), rule_alts.end(), 0);
	      if(pos != rule_alts.end())
		{
		cout << "0 found in rule : "<< k << endl;
		for(auto& r : rule_alts)
		  cout << r << ",";
		cout << endl;
		}
	    }
		      
	}

    };

  
  
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
