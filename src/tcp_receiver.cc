#include "tcp_receiver.hh"
#include "debug.hh"
#include <cstdint>

using namespace std;

void TCPReceiver::receive( TCPSenderMessage message )
{
  // Your code here.
  // debug( "unimplemented receive() called" );
  const uint64_t checkpoint = reassembler_.writer().bytes_pushed() + ISN_.has_value(); // calculate checkpoint

  if ( message.RST ) // stop connection
    reassembler_.reader().set_error();
  else if ( checkpoint > 0 && checkpoint <= UINT32_MAX && message.seqno == ISN_ )
    return;

  // first SYN ?
  if ( !ISN_.has_value() ) {
    if ( !message.SYN )
      return;
    ISN_ = message.seqno; // set first SYN
  }

  // get message absolute seq #
  const uint64_t abso_seqno_ = message.seqno.unwrap( *ISN_, checkpoint );
  // stream index = absolute seq # - 1 ( SYN not a stream message )
  // interface: void Reassembler::insert( uint64_t first_index, string data, bool is_last_substring )
  reassembler_.insert( abso_seqno_ == 0 ? abso_seqno_ : abso_seqno_ - 1, move( message.payload ), message.FIN );
}

TCPReceiverMessage TCPReceiver::send() const
{
  // Your code here.
  // debug( "unimplemented send() called" );
  const uint64_t checkpoint = reassembler_.writer().bytes_pushed() + ISN_.has_value(); // calculate checkpoint
  const uint64_t capacity = reassembler_.writer().available_capacity();                // calculate capacity
  const uint16_t wnd_size = capacity > UINT16_MAX ? UINT16_MAX : capacity;             // window size

  // first SYN?
  if ( !ISN_.has_value() )
    return { {}, wnd_size, reassembler_.writer().has_error() };
  else
    return { Wrap32::wrap( checkpoint + reassembler_.writer().is_closed(), *ISN_ ),
             wnd_size,
             reassembler_.writer().has_error() };
}
