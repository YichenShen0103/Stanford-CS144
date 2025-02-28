#include "reassembler.hh"
#include "debug.hh"
#include <algorithm>

using namespace std;

void Reassembler::update()
{
  // 拼接连续字节
  string to_push;
  auto it = buffer_.find( next_byte_ );
  while ( it != buffer_.end() ) {
    to_push.push_back( it->second );
    buffer_.erase( it );
    ++next_byte_;
    it = buffer_.find( next_byte_ ); // 仅在循环末尾查找
  }
  output_.writer().push( to_push );

  // 如果已经标记了最后一段且 buffer_ 已无剩余，关闭写入
  if ( next_byte_ == end_.second && end_.first ) {
    output_.writer().close();
  }
}

void Reassembler::insert( uint64_t first_index, string data, bool is_last_substring )
{
  debug( "insert({}, {}, {}) called", first_index, data.size(), is_last_substring );

  if ( is_last_substring ) {
    end_.first = true;
    end_.second = first_index + data.size();
  }

  // 根据当前可写容量，计算能接受的最大下标
  uint64_t max_index = output_.writer().available_capacity() + next_byte_;
  uint64_t last_index = first_index + data.size();
  uint64_t init = first_index;

  // 截断超出可用容量的数据
  if ( last_index > max_index ) {
    last_index = max_index;
  }

  // 跳过已经写入（next_byte_ 之前）的数据
  if ( first_index < next_byte_ ) {
    first_index = next_byte_;
  }

  // 将有效区间 [first_index, last_index) 的数据拷贝到 buffer_
  for ( uint64_t idx = first_index; idx < last_index; ++idx ) {
    buffer_[idx] = data[idx - init];
  }

  // 尝试更新可连续写入的内容
  update();
}

uint64_t Reassembler::count_bytes_pending() const
{
  // 返回当前缓冲区里尚未写入 ByteStream 的字节数
  return buffer_.size();
}
