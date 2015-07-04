#include "rf_protocol.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main(int argc, char const *argv[])
{
  struct packet pack=make_packet(1, 3, 0, 5, "Hello");
  byte* raw;
  int len=packet_to_bytes(&raw,pack);
  printf("packet length: %d\n", len);
  struct packet unpack;
  int i;
  for (i = 0; i < len; ++i)
  {
    printf("reading byte %d: %d\n",i,raw[i]);
    if(read_stream_to_packet(&unpack,raw[i])){
      printf("packet read after byte %d\n",i);
      char* plds=malloc(unpack.length+1);
      memcpy(plds,unpack.payload,unpack.length);
      plds[unpack.length]=0;
      printf("from %d to %d type %d len %d payload %s\n",unpack.from,unpack.to,unpack.type,unpack.length,plds);
      break;
    }
  }
  return 0;
}
