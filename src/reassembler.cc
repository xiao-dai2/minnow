#include "reassembler.hh"

using namespace std;

void Reassembler::insert( uint64_t first_index, string data, bool is_last_substring )
{
  auto siz = data.size();
  auto ava_capacity = output_.writer().available_capacity();
  // if(ava_capacity<data.size()){
  //    output_.set_error();
  //    return ;
  // }
  if ( is_last_substring )
    end_index = first_index + siz;

  if ( expected_index == first_index ) {
    q.push( { first_index, data } );
    dict.insert( first_index );
    while ( q.size() && q.top().first == expected_index ) {
      expected_index += q.top().second.size();
      output_.writer().push( q.top().second );
      dict.erase( q.top().first );
      q.pop();
    }

  } else if ( first_index > expected_index ) {
    if ( dict.count( first_index ) || expected_index + ava_capacity < first_index + siz )
      return;
    q.push( { first_index, data } );
    dict.insert( first_index );
  }

  if ( expected_index == end_index && end_index )
    output_.writer().close();
}

uint64_t Reassembler::bytes_pending() const
{
  return byte_store;
}
