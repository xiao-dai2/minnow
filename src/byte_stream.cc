#include "byte_stream.hh"
#include<iostream>
using namespace std;

ByteStream::ByteStream( uint64_t capacity ) : capacity_( capacity ) {}

bool Writer::is_closed() const
{
  
  return is_close;
}

void Writer::push( string data )
{
  auto red = available_capacity();
  if(data.size()<=red){
    for(auto c:data)
    str.push_back(c);
    pushed += data.size();
  }
  else set_error();
  
  return;
}
  //
void Writer::close()
{
  is_close = true;
}

uint64_t Writer::available_capacity() const
{
  return capacity_-str.size();
}

uint64_t Writer::bytes_pushed() const
{
  return pushed;
}

bool Reader::is_finished() const
{
  return is_close&&str.size()==0;
}

uint64_t Reader::bytes_popped() const
{
  return poped;
}

string_view Reader::peek() const
{  
  string ret{};
  ret.assign(str.begin(),str.end());

  return string_view(ret.c_str()); 
}

void Reader::pop( uint64_t len )
{
    uint64_t cnt=0;
    while (cnt<len)
    {
      str.pop_front();
      cnt++;
    }
    poped += len;
}

uint64_t Reader::bytes_buffered() const
{
  return str.size();
}
