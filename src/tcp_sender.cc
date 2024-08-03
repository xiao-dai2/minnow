#include "tcp_sender.hh"
#include "tcp_config.hh"
#include <iostream>

using namespace std;

uint64_t TCPSender::sequence_numbers_in_flight() const
{
  return seq_num;
}

uint64_t TCPSender::consecutive_retransmissions() const
{
  return conse_retran;
}

void TCPSender::push( const TransmitFunction& transmit )
{
  auto len = TCPConfig::MAX_PAYLOAD_SIZE;
  TCPSenderMessage tmp;
  auto cap = window_size_ - ( nextseq_.get_raw() - ( *ackno_ ).get_raw() );
  len = min( len, window_size_ - ( nextseq_.get_raw() - ( *ackno_ ).get_raw() ) );
  
  if ( window_size_ == 0 && is_first) return;
  //   len = 1;

  if ( over )  return; //

  while (len&&!over)
  {
      uint64_t lent=0;
      if ( !is_first ) {
        is_first = true;
        tmp.SYN = true;
        tmp.seqno = nextseq_;
      } else {
        
        lent = min( len, input_.reader().bytes_buffered() );
        tmp.seqno = nextseq_;

        read( input_.reader(), lent, tmp.payload );
        is_finish = input_.reader().is_finished();
        
        if ( is_finish && cap - lent )
          goto ext;
        if ( lent == 0 ) {
          return; // attention FIN flag bug!!!
        }
      }
      is_finish = input_.reader().is_finished();
    ext:
      if ( is_finish && cap - lent  ) {
        tmp.FIN = true;
        over = true;
      }
      nextseq_ = nextseq_ + tmp.sequence_length();

      seq_num += tmp.sequence_length();
      q.push_back( tmp );
      transmit( tmp );
      
      if ( !runnning ) {
        runnning = true;
        time_ = 0;
      }
      if(window_size_< nextseq_.get_raw() - ( *ackno_ ).get_raw()) break;

      len = min( len, window_size_ - ( nextseq_.get_raw() - ( *ackno_ ).get_raw() ) );

  }
}

TCPSenderMessage TCPSender::make_empty_message() const
{
  TCPSenderMessage ret;
  ret.seqno = nextseq_;
  return ret;
}

void TCPSender::receive( const TCPReceiverMessage& msg )
{
  if ( nextseq_ < msg.ackno ) {
    return;
  }
  auto ackno = msg.ackno;
  window_size_ = msg.window_size;
  ackno_ = msg.ackno;

  uint64_t cnt = 0;

  while ( q.size() ) {
    auto tmp = q.front();
    if ( tmp.seqno + tmp.sequence_length() <= ackno ) {
      seq_num -= q.front().sequence_length();
      q.pop_front();
      cnt++;

    } else
      break;
  }
  if ( cnt ) {
    RTO_ms_ = initial_RTO_ms_;
    conse_retran = 0;
    if ( q.size() ) {
      time_ = 0;
      runnning = true;
     
    } else {
      runnning = false;
      time_ = 0;
    
    }
  }


}

void TCPSender::tick( uint64_t ms_since_last_tick, const TransmitFunction& transmit )
{

  time_ += ms_since_last_tick;
  if ( time_ >= RTO_ms_ && runnning ) {
    transmit( q.front() );
    // if ( window_size_ ) {
    conse_retran++;
    RTO_ms_ *= 2;
    //  }
    // when window_size_==0
    // else {}

    runnning = true;
    time_ = 0;
  }
}
