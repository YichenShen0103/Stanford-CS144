#include "byte_stream.hh"
#include <stdexcept>

using namespace std;

ByteStream::ByteStream( uint64_t capacity ) : capacity_( capacity ) {}

void Writer::push( string data ) noexcept
{
  auto len = min( data.size(), available_capacity() ); // 确定可写入的数据长度
  if ( len == 0 ) {                                    // 如果可写入的数据长度为0，说明已经写满了，返回
    return;
  } else if ( len < data.size() ) { // 如果可写入的数据长度小于 data 的长度，说明只能写入部分数据
    data.resize( len );             // 将 data 的长度截断为可写入的长度
  }
  // 将 data 写入到 buffer 中
  buffer_data.push( move( data ) );
  if ( buffer_data.size() == 1 ) // 写入前为空时需要更新 buffer_view
    buffer_view = buffer_data.front();
  // 更新已写入的数据长度
  bytes_pushed_ += len;
}

void Writer::close() noexcept
{
  flag |= ( 1 << CLOSED );
}

void Writer::set_error() noexcept
{
  flag |= ( 1 << ERROR );
}

bool Writer::is_closed() const noexcept
{
  return flag & ( 1 << CLOSED );
}

uint64_t Writer::available_capacity() const noexcept
{
  return capacity_ - reader().bytes_buffered();
}

uint64_t Writer::bytes_pushed() const noexcept
{
  return bytes_pushed_;
}

string_view Reader::peek() const noexcept
{
  return buffer_view;
}

bool Reader::is_finished() const noexcept
{
  return writer().is_closed() && ( bytes_buffered() == 0 );
}

bool Reader::has_error() const noexcept
{
  return flag & ( 1 << ERROR );
}

void Reader::pop( uint64_t len ) noexcept
{
  if ( len > bytes_buffered() ) {
    return;
  }
  // 更新已弹出的数据长度
  bytes_popped_ += len;

  // 将 buffer 中的数据弹出
  while ( len > 0 ) {
    if ( len >= buffer_view.size() ) {
      len -= buffer_view.size();
      buffer_data.pop();
      buffer_view = buffer_data.front(); // 最开始就保证了 buffer_data 不为空
    } else {
      buffer_view.remove_prefix( len );
      len = 0;
    }
  }
}

uint64_t Reader::bytes_buffered() const noexcept
{
  return writer().bytes_pushed() - bytes_popped();
}

uint64_t Reader::bytes_popped() const noexcept
{
  return bytes_popped_;
}
