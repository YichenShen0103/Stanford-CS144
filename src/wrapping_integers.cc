#include "wrapping_integers.hh"
#include "debug.hh"

#include <cmath>
using namespace std;

Wrap32 Wrap32::wrap( uint64_t n, Wrap32 zero_point )
{
  // Your code here.
  // debug( "unimplemented wrap( {}, {} ) called", n, zero_point.raw_value_ );
  return Wrap32 { (uint32_t)( zero_point.raw_value_ + n ) };
}

uint64_t Wrap32::unwrap( Wrap32 zero_point, uint64_t checkpoint ) const
{
  // Your code here.
  // debug( "unimplemented unwrap( {}, {} ) called", zero_point.raw_value_, checkpoint );
  constexpr uint64_t upper = static_cast<uint64_t>( UINT32_MAX ) + 1;
  const uint32_t ckpt_mod = Wrap32::wrap( checkpoint, zero_point ).raw_value_;
  uint32_t distance = raw_value_ - ckpt_mod;
  if ( distance <= ( upper >> 1 ) || checkpoint + distance < upper )
    return checkpoint + distance;
  return checkpoint + distance - upper;
}
