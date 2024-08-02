#include "tcp_sender.hh"
#include "tcp_config.hh"

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
    len = min(len,window_size_);
    len = max(len,1UL);
    
       if(!is_first){
            is_first = true;
            tmp.SYN = true;
            tmp.seqno = nextseq_;
        }
        else{
            read(input_.reader(),min(len,input_.reader().bytes_buffered()),tmp.payload);
            tmp.seqno = nextseq_;
        }
        if(input_.reader().is_finished()) tmp.FIN = true;
        nextseq_ = nextseq_+tmp.sequence_length();
        transmit(tmp);
    
}

TCPSenderMessage TCPSender::make_empty_message() 
{
    TCPSenderMessage ret;
    ret.seqno = nextseq_;
    nextseq_  = nextseq_ +1;
    return ret;
}

void TCPSender::receive( const TCPReceiverMessage& msg )
{
 
    auto ackno = msg.ackno;
    auto window_size=msg.window_size;
    window_size_ = msg.window_size;
    ackno_ = msg.ackno;
    uint64_t cnt = 0;
    while (q.size())
    {
        auto tmp=q.front();
        if(tmp.seqno+tmp.sequence_length()<=ackno){
          q.pop_front();
          cnt++;
        }
        else break;
    }
    if(cnt){
      RTO_ms_=initial_RTO_ms_;
      conse_retran=0;
      if(q.size()){
        time_ = 0;
        runnning = true;
      } 
      else {
        runnning = false;
        time_= 0;   
      } 
    }

}

void TCPSender::tick( uint64_t ms_since_last_tick, const TransmitFunction& transmit )
{
  // // Your code here.
  // (void)ms_since_last_tick;
  // (void)transmit;
  // (void)initial_RTO_ms_;
  time_ += ms_since_last_tick;
  if(time_>=RTO_ms_&&runnning){
      transmit(q.front());    
      if(window_size_){
         conse_retran++;
         RTO_ms_*=2;
      }
      // when window_size_==0 
      else{
 


      }

      runnning=true;
      time_ = 0;
  }

  

}
