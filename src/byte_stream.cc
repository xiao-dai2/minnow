#include "byte_stream.hh"
#include <iostream>
using namespace std;

ByteStream::ByteStream( uint64_t capacity ) : capacity_( capacity ) {}

bool Writer::is_closed() const
{

  return is_close;
}

void Writer::push( string data )
{
  auto red = available_capacity();
  auto add = min( red, data.size() );
  for ( uint64_t i = 0; i < add; i++ )
    str.push_back( data[i] );
  pushed += add;
 // cout<<pushed<<endl;//
  return;
}
//
void Writer::close()
{
  is_close = true;
}

uint64_t Writer::available_capacity() const
{
  return capacity_ - str.size();
}

uint64_t Writer::bytes_pushed() const
{
  return pushed;
}

bool Reader::is_finished() const
{
  return is_close && str.size() == 0;
}

uint64_t Reader::bytes_popped() const
{
  return poped;
}

string_view Reader::peek() const
{
  return string_view( str );
}

void Reader::pop( uint64_t len )
{
  //  uint64_t cnt=0;
  auto rec = bytes_buffered();
  if ( len > rec ) {
    set_error();
    return;
  }
  str.erase( 0, len );
  poped += len;
}

uint64_t Reader::bytes_buffered() const
{
  return str.size();
}
