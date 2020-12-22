#include <iostream>
#include <string>
#include <fstream>
#include <deque>
#include <tuple>
#include <algorithm>
#include <numeric>
#include <vector>

using std::cout;
using std::endl;
using std::string;

std::tuple<std::deque<int>,std::deque<int>> parse_decks(std::ifstream& ifs)
{
  string line;
  std::deque<int> p1deck;
  std::deque<int> p2deck;
  int playernum;
  while(std::getline(ifs, line))
    {
      if(line[0] == 'P')
	{
	  //player definition
	  auto delimpos = line.find(':');
	  playernum = std::stoi(line.substr(delimpos-1, delimpos));
	  continue;
	};
      line.erase(line.begin(), std::find_if(line.begin(), line.end(),[](unsigned char uc){return !std::isspace(uc);}));
      if(line.size() ==0)
	continue;
      int num = std::stoi(line);
      if(playernum == 1)
	p1deck.push_back(num);
      else if(playernum ==2)
	p2deck.push_back(num);
      else
	throw std::logic_error("logic error!");
     
    }
  return {p1deck, p2deck};
};


int run_turn(std::deque<int>& p1deck, std::deque<int>& p2deck)
{
  auto c1 = p1deck.front();
  p1deck.pop_front();
  auto c2 = p2deck.front();
  p2deck.pop_front();
  int winner = (c1 > c2) ? 1 : 2;

  if(winner ==1)
    {
      p1deck.push_back(c1);
      p1deck.push_back(c2);
    }
  else
    {
      p2deck.push_back(c2);
      p2deck.push_back(c1);
    }

  return winner;
};


int calculate_score(const std::deque<int>& winning_deck)
{
  std::vector<int> weights;
  weights.resize(winning_deck.size());
  std::iota(weights.begin(), weights.end(),1);

  for(auto& i: weights)
    cout << i << ",";
  cout << endl;
  
  int score = std::inner_product(weights.begin(),weights.end(), winning_deck.rbegin(), 0);
  return score;
};

int main(int argc, char** argv)
{
  std::ifstream ifs("input.txt");
  auto [p1deck, p2deck] = parse_decks(ifs);

  int turn = 0;
  while(p1deck.size() != 0 && p2deck.size() != 0)
    {
      cout << "turn: " << ++turn << endl;
      run_turn(p1deck, p2deck);
    };

  int score;
  if(p1deck.size() ==0)
    score = calculate_score(p2deck);
  else
    score = calculate_score(p1deck);

  for(auto& c2 : p2deck)
    cout << c2 << ",";
  cout << endl;

  cout << "winnder's score:" << score << endl;

}
