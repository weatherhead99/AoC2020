#include <iostream>
#include <vector>
#include <map>
#include <tuple>
#include <string>
#include <fstream>
#include <set>

using std::cout;
using std::endl;
using std::string;

using ruleset =   std::map<string,std::vector<std::tuple<int, int>>>;

struct tickets
{
  ruleset rules;
  std::vector<int> myticket;
  std::vector<std::vector<int>> nearby_tickets;
};

std::tuple<int,int> parse_group(const string& group)
{
  auto pivot = group.find("-");
  auto num1 = std::stoi(group.substr(0,pivot));
  auto num2 = std::stoi(group.substr(pivot+1));
  //cout << "num1: " << num1 << " num2: " << num2 << endl;
  return {num1, num2};
};

std::vector<int> parse_comma_separated(const string& line)
{
  auto start = 0;
  auto delimpos = line.find(",");
  std::vector<int> out;
  while(delimpos != string::npos)
    {
      auto valstr = line.substr(start, delimpos - start);
      out.push_back(std::stoi(valstr));
      start = delimpos+1;
      delimpos = line.find(",", start);
    };
  auto valstr = line.substr(start, delimpos-start);
  out.push_back(std::stoi(valstr));
  return out;
};

tickets parse_tickets(std::ifstream& ifs)
{
  string line;
  tickets out;
  std::getline(ifs, line);
  while(line != "")
    {
      //parse rules
      auto delimpos = line.find(":");
      auto nm = line.substr(0,delimpos);
      
      auto groupsplit = line.find(" or ");
      auto group1 = line.substr(delimpos+2, groupsplit - delimpos-1);
      auto rule1 = parse_group(group1);
      out.rules[nm].push_back(rule1);
      
      auto group2 = line.substr(groupsplit+4);
      auto rule2 = parse_group(group2);
      out.rules[nm].push_back(rule2);
      
      std::getline(ifs,line);
    };

  //parse my tickets
  std::getline(ifs, line);
  std::getline(ifs, line);
  out.myticket = parse_comma_separated(line);

  //parse nearby tickets
  std::getline(ifs,line); //empty line
  std::getline(ifs,line); // "nearby tickets: " 
  while(std::getline(ifs, line))
      out.nearby_tickets.push_back(parse_comma_separated(line));
  
  return out;
};

bool check_rule(int v, const std::vector<std::tuple<int,int>>& rule)
{
  bool valid = false;
  for(auto& [v1, v2] : rule)
    {
      bool check = !((v < v1) || (v > v2));
      valid = valid || check;
    };
  return valid;
}


bool test_value_valid(const ruleset& rules, int v)
{
  //  cout << "value: " << v << endl;
  bool valid = false;
  for(auto& [name, rule]: rules)
    {
      bool rulevalid = check_rule(v,rule);
      valid = valid || rulevalid;
    };
  return valid;
};


template<typename Container>
bool any_not_size_1(const Container& cont)
{
  for(auto& [k,v]: cont)
    if(v.size() != 1)
      return true;
  return false;
}


void filter_unique_fields(std::map<int,std::set<string>>& possible_fields)
{
  for(auto& [pos, fieldset] : possible_fields)
    {
      if(fieldset.size() == 1)
	{
	  //only one possible field, filter it out of all the others
	  for(auto& [pos2, fieldset2]: possible_fields)
	    {
	      if(pos == pos2)
		continue;
	      fieldset2.erase(*fieldset.begin());
	    }
	}
    }
}
  


int main(int argc, char** argv)
{
  std::ifstream ifs("input.txt");
  auto tckts = parse_tickets(ifs);
  cout << "total tickets: " << tckts.nearby_tickets.size() << endl;
  std::vector<std::vector<int>> valid_tickets;
  valid_tickets.reserve(tckts.nearby_tickets.size());
  int sm = 0;
  for(auto& tick: tckts.nearby_tickets)
    {
      bool any_invalid = false;
      for(int val: tick)
	{
	  bool valid = test_value_valid(tckts.rules, val);
	  if(not valid)
	    {
	      sm += val;
	      any_invalid = true;
	    }
	}
      if(not any_invalid)
	valid_tickets.push_back(tick);
    }

  cout << "sum of invalid values: " << sm << endl;


  std::vector<string> rule_names;
  rule_names.reserve(tckts.rules.size());
  for(auto& v : tckts.rules)
    rule_names.push_back(v.first);
  
  
  std::map<int, std::set<string>> possible_fields;
  for(int i=0; i < valid_tickets[0].size(); i++)
    possible_fields[i].insert(rule_names.begin(), rule_names.end());

  cout << "there are : " << valid_tickets.size() << " valid tickets" << endl;
  //filter possible fields based on name criteria
  for(auto& ticket: valid_tickets)
    {
      for(int i=0; i < ticket.size(); i++)
	{
	  for(auto& [name, rule]: tckts.rules)
	    {
	      auto valid = check_rule(ticket[i], rule);
	      if(!valid)
		possible_fields[i].erase(name);
	    }
	}
    }

  cout << "##### printing all valid field name possibilities##### " << endl;
    for(int i=0; i < valid_tickets[0].size(); i++)
    {
      cout << "i: " << i << endl;
      for(auto& k : possible_fields[i])
	    cout << k  << ",";
      cout << endl;
      cout << "-----------------" << endl;
    }

  
  
  for(auto& [pos, fieldset] : possible_fields)
    {
      if(fieldset.size() == 1)
	{
	  //only one possible field, filter it out of all the others
	  for(auto& [pos2, fieldset2]: possible_fields)
	    {
	      if(pos == pos2)
		continue;
	      fieldset2.erase(*fieldset.begin());
	    }
	}
    }

  int i=0;
  while(any_not_size_1(possible_fields))
    {
      cout << "iteration: " << i++ << endl;
      filter_unique_fields(possible_fields);
    };

  cout << "#### printing valid possibilities after filtering#### " << endl;

  std::map<int, string> fields_map;
  for(auto& [k, v] : possible_fields)
    {
      fields_map[k] = *v.begin();
    }
  
  for(auto&[k,v] : fields_map)
    cout << "position: " << k << ", field: " << v << endl;
  
  long int product = 1;
  for(auto&[k,v] : fields_map)
    {
      if(v.find("departure") == 0)
	{
	  cout << "found field: " << v << endl;
	  cout << "at position: " << k << endl;
	  product *= tckts.myticket[k];
	}
    }
  
  cout << "product: " << product << endl;
  
}
