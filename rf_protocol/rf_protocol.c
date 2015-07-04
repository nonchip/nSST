#include <string.h>
#include <stdlib.h>
#include "rf_protocol.h"
#include "rf_protocol.ycdef.h"

byte calculate_checksum(struct packet p){
  byte sum
    =p.magic[0]+p.magic[1]+p.magic[2]+p.magic[3]
    +p.version[0]+p.version[1]
    +p.from
    +p.to
    +p.type
    +p.length;
  int i;
  for (i = 0; i < p.length; ++i)
  {
    sum+=p.payload[i]; //this'll wrap on overflow according to C99
  }
  return sum;
}

struct packet make_packet(byte from, byte to, byte type, byte length, byte* payload){
  struct packet out;
  memcpy(out.magic,PACKET_MAGIC,4);
  out.version[0]=VERSION_MAJOR;
  out.version[1]=VERSION_MINOR;
  out.from=from;
  out.to=to;
  out.type=type;
  out.length=length;
  out.payload=malloc(length);
  memcpy(out.payload,payload,length);
  out.checksum=calculate_checksum(out);
  out.end=0;
  return out;
}

int read_stream_to_packet_state=0;
int read_stream_to_packet_startcount=0;
int read_stream_to_packet_len=0;
struct packet read_stream_to_packet_temp;
int read_stream_to_packet(struct packet* outp,byte b){
  if(read_stream_to_packet_startcount==0 && b=='n')
    read_stream_to_packet_startcount=1;
  else if(read_stream_to_packet_startcount==1 && b=='S')
    read_stream_to_packet_startcount=2;
  else if(read_stream_to_packet_startcount==2 && b=='S')
    read_stream_to_packet_startcount=3;
  else if(read_stream_to_packet_startcount==3 && b=='T'){
    read_stream_to_packet_startcount=0;
    read_stream_to_packet_state=1;
    memcpy(read_stream_to_packet_temp.magic,PACKET_MAGIC,4);
  }
  else if(read_stream_to_packet_state==1){
    read_stream_to_packet_temp.version[0]=b;
    read_stream_to_packet_state=2;
  }
  else if(read_stream_to_packet_state==2){
    read_stream_to_packet_temp.version[1]=b;
    read_stream_to_packet_state=3;
  }
  else if(read_stream_to_packet_state==3){
    read_stream_to_packet_temp.from=b;
    read_stream_to_packet_state=4;
  }
  else if(read_stream_to_packet_state==4){
    read_stream_to_packet_temp.to=b;
    read_stream_to_packet_state=5;
  }
  else if(read_stream_to_packet_state==5){
    read_stream_to_packet_temp.type=b;
    read_stream_to_packet_state=6;
  }
  else if(read_stream_to_packet_state==6){
    read_stream_to_packet_temp.length=b;
    read_stream_to_packet_state=7;
    read_stream_to_packet_temp.payload=malloc(b);
    read_stream_to_packet_len=0;
  }
  else if(read_stream_to_packet_state==7){
    read_stream_to_packet_temp.payload[read_stream_to_packet_len++]=b;
    if(read_stream_to_packet_len>=read_stream_to_packet_temp.length)
      read_stream_to_packet_state=8;
  }
  else if(read_stream_to_packet_state==8){
    read_stream_to_packet_temp.checksum=b;
    read_stream_to_packet_state=9;
  }
  else if(read_stream_to_packet_state==9){
    read_stream_to_packet_state=0;
    read_stream_to_packet_temp.end=b;
    if(read_stream_to_packet_temp.end==0 && read_stream_to_packet_temp.checksum==calculate_checksum(read_stream_to_packet_temp)){
      memcpy(outp,&read_stream_to_packet_temp,sizeof(struct packet));
      return 1;
    }
  }
  outp=0;
  return 0;
}

int packet_to_bytes(byte** outb,struct packet p){
  int len=12+p.length;
  *outb=malloc(len);
  memcpy(*outb,p.magic,4);
  memcpy(*outb+4,p.version,2);
  (*outb)[6]=p.from;
  (*outb)[7]=p.to;
  (*outb)[8]=p.type;
  (*outb)[9]=p.length;
  memcpy(*outb+10,p.payload,p.length);
  (*outb)[10+p.length]=p.checksum;
  (*outb)[11+p.length]=p.end;
  return len;
}
