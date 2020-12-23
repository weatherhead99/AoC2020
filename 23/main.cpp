#include <iostream>
#include <list>
#include <string>
#include <vector>
#include <algorithm>
#include <boost/iterator/iterator_adaptor.hpp>

using std::cout;
using std::endl;
using std::string;



template<typename Iter>
class circular_iterator : public boost::iterator_adaptor<circular_iterator<Iter>, Iter,
							 boost::use_default,
							 boost::forward_traversal_tag>
{
  
private:
  Iter it_end;
  Iter it_begin;
public:
  circular_iterator(Iter beg, Iter end, Iter current):
    circular_iterator::iterator_adaptor_(beg), it_begin(beg), it_end(end)
  {
    circular_iterator::base_reference() = current;
  };

  void increment()
  {
    if(++this->base_reference() == this->it_end)
      {
	this->base_reference() = it_begin;
      }
    
  };

  void advance(std::size_t d)
  {
    for(std::size_t i=0; i < d; i++)
      increment();
  }
  
};

void do_move(std::list<unsigned char>& cups, std::list<unsigned char>::iterator& current_cup)
{
  cout << "current cup: " << (int) *current_cup << endl;
  cups.splice(cups.end(), cups, cups.begin(), std::next(current_cup,-1));
  
  
  auto pickup_begin = std::next(current_cup, 1);
  auto pickup_end = std::next(current_cup, 4);

  
  std::vector<unsigned char> picked_up(pickup_begin, pickup_end);
    cout << "pickup: " << endl;
  for(auto& pu: picked_up)
    cout << (int) pu <<  ",";
  cout << endl;


  
  cups.erase(pickup_begin, pickup_end);
  auto dest_target = *current_cup - 1;
  if(dest_target ==0)
    dest_target = 9;
  while(std::find(picked_up.begin(), picked_up.end(), dest_target) != picked_up.end())
    {
      //wrap around;
      if(dest_target-1 == 0 )
	dest_target = 9;
      else
	dest_target--;
    }
  cout << "destination: " << dest_target << endl;
  auto target_it = std::find(cups.begin(), cups.end(), dest_target);
  cups.insert(std::next(target_it,1), picked_up.cbegin(), picked_up.cend());



  
  current_cup++;
  
};

std::vector<unsigned char> canonical_arrangement(std::list<unsigned char>& cups)
{
  std::vector<unsigned char> out;
  out.reserve(cups.size());
  auto pos1 = std::find(cups.begin(), cups.end(), 1);
  auto cit = circular_iterator(cups.begin(), cups.end(), ++pos1);
  for(int i=0; i < cups.size(); i++)
    out.push_back(*cit++);
  return out;
};


int main(int argc, char** argv)
{
  //std::vector<unsigned char> labels{3,8,9,1,2,5,4,6,7};
  std::vector<unsigned char> labels{8,5,3,1,9,2,6,4,7};
  std::list<unsigned char> listlabels{labels.begin(), labels.end()};

  for(auto& v : listlabels)
	cout << (int) v << ",";
      cout << endl;

      int nturns = 100;
  auto current_cup = listlabels.begin();
  for(int i=0; i < nturns; i++)
    {
      do_move(listlabels, current_cup);

      for(auto& v : listlabels)
	cout << (int) v << ",";
      cout << endl;
    }
  
  auto arrange = canonical_arrangement(listlabels);
  for(auto& v: arrange)
    cout << (int) v;
  cout << endl;
  
}
