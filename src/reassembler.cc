#include "reassembler.hh"
#include <iostream>
#include <algorithm>
using namespace std;

void Reassembler::insert( uint64_t first_index, string data, bool is_last_substring )
{
  if(first_index+data.size()>expected_index&&first_index<expected_index){
      data = data.substr(expected_index-first_index);
      first_index = expected_index;    
  }

  auto siz = data.size();
  auto ava_capacity = output_.writer().available_capacity();
  priority_queue<pair<uint64_t, string>, vector<pair<uint64_t, string>>, greater<>> qq{}; 

  if ( is_last_substring )
    end_index = first_index + siz;
 
  if ( expected_index == first_index ) {
    q.push({ first_index, data });
    dict.push_back({first_index,min(first_index+data.size(),expected_index+ava_capacity)});
    
    while ( q.size() && q.top().first <= expected_index) {
      if(q.top().first+q.top().second.size()<=expected_index){
          q.pop();
          continue;
      }
      string str = q.top().second;
      auto id = q.top().first;
      str = str.substr(expected_index-id);
      id = expected_index;
      expected_index += str.size();
   //   byte_store -= str.size();
      auto pre = output_.writer().available_capacity();
      output_.writer().push(str);
      //dict.erase(id);
      ava_capacity = output_.writer().available_capacity();
      if(ava_capacity==0){
        q = qq;
        dict.clear();
       //byte_store=0;
        expected_index -= str.size();
        expected_index += pre;
        break;
      }
      q.pop();

    }

  } else if ( first_index > expected_index ) {
    
    if (first_index>=expected_index+ava_capacity) return;
    if( expected_index + ava_capacity < first_index + siz ){
        data = data.substr(0,expected_index+ava_capacity-first_index);
        siz = data.size();
    }
    q.push( { first_index, data } );
    dict.push_back({first_index,min(first_index+data.size(),expected_index+ava_capacity)});
    //byte_store += siz;
  }
  if ( expected_index == end_index)
    output_.writer().close();
 
}

uint64_t Reassembler::bytes_pending() const
{

  vector<pair<uint64_t,uint64_t>> tmp{};
  for(auto [l,r]:dict)
     if(l>expected_index) tmp.push_back({l,r});
  uint64_t ret=0;
  sort(tmp.begin(),tmp.end(),less<>());

  if(tmp.empty()) return ret;
  uint64_t l=tmp[0].first,r=tmp[0].second;
  for(uint64_t i=0;i<tmp.size();i++){
      if(r>=tmp[i].first){
        r=max(r,tmp[i].second);
      }
      else{
        ret += r-l;
        l = tmp[i].first;
        r = tmp[i].second;
      }
  }
  ret += r-l;

  return ret;
}
