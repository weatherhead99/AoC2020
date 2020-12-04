#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <fstream>
#include <variant>
#include <vector>
#include <string>
#include <algorithm>
#include <array>
#include <regex>


using std::cout;
using std::endl;
using std::string;
using std::vector;

using entry = std::unordered_map<string, string>;

auto int_validator_factory(int min, int max)
{
  return [min, max](int v) { return (v >= min) && (v <=max);};
};

auto int_validator_str_factory(int min, int max)
{
  auto out = int_validator_factory(min,max);
  return [out](const string& st) { return out(std::stoi(st));};
};

auto byr_validate = int_validator_str_factory(1920,2002);
auto iyr_validate = int_validator_str_factory(2010,2020);
auto eyr_validate = int_validator_str_factory(2020,2030);

auto regex_validator_str_factory(const std::regex& re)
{
  auto out = [&re] (const string& s)
    {
      std::smatch m;
      auto match = std::regex_match(s.cbegin(), s.cend(), m, re);
      return match;
    };
  return out;
};


auto hgt_regex = std::regex("([0-9]+)(cm|in)");
bool hgt_validate(const string& hgt)
{
  std::smatch m;
  auto re_match = std::regex_match(hgt.cbegin(), hgt.cend(), m, hgt_regex);
  if(!re_match)
    return false;

  auto units = m[2];
  auto val = std::stoi(m[1]);
  if(units == "cm")
    return int_validator_factory(150,193)(val);
  else if(units == "in")
    return int_validator_factory(59,76)(val);
  else
    throw std::logic_error("match fail!");

};

auto hcl_regex = std::regex("#[0-9a-f]{6}");
auto hcl_validate = regex_validator_str_factory(hcl_regex);




std::unordered_set<string> valid_eye_colours = {"amb", "grn", "blu", "brn", "gry", "hzl", "oth"};
bool ecl_validate(const string& ecl)
{
  if(valid_eye_colours.find(ecl) == valid_eye_colours.end())
    return false;
  return true;
}


auto pid_regex = std::regex("[0-9]{9}");
auto pid_validate = regex_validator_str_factory(pid_regex);



std::unordered_map<string, std::function<bool(const string&)>> validators = { {"hcl", hcl_validate},
									      {"pid", pid_validate},
									      {"hgt", hgt_validate},
									      {"byr", byr_validate},
									      {"iyr", iyr_validate},
									      {"eyr", eyr_validate},
									      {"ecl", ecl_validate}};




void parse_kv_into_map(const string& str, entry& map)
{
  auto splitpos = str.find(":");
  if(splitpos == string::npos)
    throw std::runtime_error("invalid entry!");
  auto key  = str.substr(0,splitpos);
  auto val = str.substr(splitpos+1,str.size());
  map.emplace(key, val);

}


vector<entry> load_file()
{
  std::ifstream ifs("input.txt");
  string line;
  entry current_entry;
  vector<entry> out;
  string DELIMS = " ";
  
  while(std::getline(ifs, line))
    {
      if(line.size() == 0)
	{
	  // for(auto& i : current_entry)
	  //   cout << "key: `" << i.first << "`, value: `" << i.second << "`" << endl;

	  //this entry is finished
	  cout << "************entry finished*********" << endl;
	  out.push_back(current_entry);
	  current_entry.clear();
	  continue;
	}

      auto splitpos_start = 0;
      auto splitpos_end = line.find_first_of(DELIMS, 0);
      cout << "line: " << line << endl;
      string estr;
      while(splitpos_end != string::npos)
	{
	  estr = line.substr(splitpos_start, splitpos_end-splitpos_start);
	  cout << "estr: " << estr << endl;
	  splitpos_start = splitpos_end+1;
	  splitpos_end = line.find_first_of(DELIMS, splitpos_start);
	  parse_kv_into_map(estr, current_entry);
	}
      estr = line.substr(splitpos_start, splitpos_end-splitpos_start);
      parse_kv_into_map(estr, current_entry);
      cout << "---------------"<< endl;
    }
  return out;
};

std::array<string, 7> keys_required = {"byr", "iyr", "eyr", "hgt",
				       "hcl", "ecl", "pid"};
bool validate_entry_part1(const entry& e)
{
  for(auto& reqkey: keys_required)
    if(e.find(reqkey) == e.end())
      {
	// cout << "----------------------" <<endl;
	// cout << "missing key: " << reqkey << endl;
	// for(auto& kv : e)
	//   {
	//     cout << "k: " << kv.first << ", v: " << kv.second << endl;
	//   }
	// cout << "-----------------------" << endl;
	return false;
      }
  return true;
}


bool validate_entry_part2(const entry& entry)
{
  for(auto& reqkey: keys_required)
    {
      auto item = entry.find(reqkey);
      cout << "key: " << reqkey << "..." << endl;
      if(item  == entry.end())
	{
	  cout << "missing... "<< endl;
	  return false;
	}
      else if(validators.at(reqkey)(item->second) == false)
	{
	  cout << item->second <<  " failed validation" << endl;
	  return false;
	}
    }
  return true;
}
  
int main(int argc, char** argv)
{
  auto entries_vec =   load_file();
  cout << "number of entries: " << entries_vec.size() << endl;

  auto valid_passports_part1 = std::count_if(entries_vec.begin(), entries_vec.end(),validate_entry_part1);
  cout << "valid passports part 1: " << valid_passports_part1 << endl;

  auto valid_passports_part2 = std::count_if(entries_vec.begin(), entries_vec.end(), validate_entry_part2);
  cout << "valid passports part 2: " << valid_passports_part2 << endl;
}
