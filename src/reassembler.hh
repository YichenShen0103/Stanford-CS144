#pragma once

#include "byte_stream.hh"
#include <algorithm>
#include <cmath>
#include <iostream>
#include <set>
#include <string>

struct Interval
{
  uint64_t start;
  uint64_t end;
  std::string data;

  bool operator<( const Interval& other ) const
  {
    if ( start == other.start ) {
      return end < other.end;
    }
    return start < other.start;
  }
};

class Reassembler
{
public:
  // Construct Reassembler to write into given ByteStream.
  explicit Reassembler( ByteStream&& output ) : output_( std::move( output ) ) {}

  void insert( uint64_t first_index, std::string data, bool is_last_substring );

  // How many bytes are stored in the Reassembler itself?
  uint64_t count_bytes_pending() const;

  // Access output stream reader
  Reader& reader() { return output_.reader(); }
  const Reader& reader() const { return output_.reader(); }

  // Access output stream writer, but const-only (can't write from outside)
  const Writer& writer() const { return output_.writer(); }

private:
  ByteStream output_; // the Reassembler writes to this ByteStream
  std::set<Interval> buf_ {};
  uint64_t nxt_expected_idx_ = 0;
  uint64_t eof_idx_ = UINT64_MAX;
};
