#include "wrapping_integers.hh"
#include<vector>
#include<cmath>
#include<iostream>
using namespace std;

Wrap32 Wrap32::wrap( uint64_t n, Wrap32 zero_point )
{
  return Wrap32{uint32_t((zero_point.raw_value_+(n<<32>>32))<<32>>32)};
}

uint64_t Wrap32::unwrap( Wrap32 zero_point, uint64_t checkpoint ) const
{
  zero_point.raw_value_ = (((long long)(raw_value_)-(long long)zero_point.raw_value_)+(1UL<<32))<<32>>32;

  uint64_t tmp = checkpoint>>32;

  uint64_t a=zero_point.raw_value_+(tmp<<32),b = zero_point.raw_value_+((tmp+1)<<32);
  uint64_t c = zero_point.raw_value_;
  if(tmp>=1)
  c = zero_point.raw_value_+((tmp-1)<<32);
  uint64_t ret=a;
  if(max(ret,checkpoint)-min(ret,checkpoint)>max(b,checkpoint)-min(b,checkpoint)) 
    ret=b;
  if(max(ret,checkpoint)-min(ret,checkpoint)>max(c,checkpoint)-min(c,checkpoint)) 
    ret=c;
  return ret;
}
