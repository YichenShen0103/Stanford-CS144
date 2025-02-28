#include "reassembler.hh"
#include "debug.hh"

using namespace std;

void Reassembler::update()
{
  if ( buffer_start_ != next_byte_ )
    return;
  std::string to_push = "";
  while ( buffer_.find( next_byte_ ) != buffer_.end() ) {
    to_push.push_back( buffer_[next_byte_] );
    buffer_.erase( next_byte_ );
    next_byte_++;
  }
  output_.writer().push( to_push );
  debug( "update called" );
}

void Reassembler::insert( uint64_t first_index, string data, bool is_last_substring )
{
  debug( "unimplemented insert({}, {}, {}) called", first_index, data, is_last_substring );
  if ( is_last_substring )
    end_ = true;
  uint64_t max_index = output_.writer().available_capacity() + next_byte_;
  uint64_t last_index = first_index + data.length();
  uint64_t init = first_index;
  if ( first_index < next_byte_ )
    first_index = next_byte_;
  if ( last_index > max_index )
    last_index = max_index;
  if ( first_index < buffer_start_ )
    buffer_start_ = first_index;

  while ( first_index < last_index ) {
    buffer_[first_index] = data[first_index - init];
    first_index++;
  }

  update();
  if ( end_ )
    output_.writer().close();
}

// How many bytes are stored in the Reassembler itself?
// This function is for testing only; don't add extra state to support it.
uint64_t Reassembler::count_bytes_pending() const
{
  // debug( "unimplemented count_bytes_pending() called" );
  return {};
}
