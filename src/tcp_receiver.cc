#include "tcp_receiver.hh"

using namespace std;

void TCPReceiver::receive( TCPSenderMessage message )
{
  if(message.SYN){
     raw_value = message.seqno;
  }
  if(!raw_value) {
    reader().set_error();//
    return;
  }
  uint64_t first_index= message.seqno.unwrap(*raw_value,ackno);
  string data = message.payload;
  bool is_last_substring = message.FIN;
 
  if(ackno&&first_index==0)  return; 
  reassembler_.insert(first_index==0?0:first_index-1,data,is_last_substring);
  ackno = reassembler().get_exp()+1;
  if(reassembler().writer().is_closed()) ackno++;
  

}

TCPReceiverMessage TCPReceiver::send() const
{
  TCPReceiverMessage ret{};
  ret.window_size = min(writer().available_capacity(),65535UL);
  if(raw_value){
    ret.ackno = (*raw_value).wrap(ackno,*raw_value);
  }else{
    ret.ackno =  std::optional<Wrap32> {};

  }
  if(reassembler().reader().has_error()) ret.RST=true;

  return ret;
}
