#ifndef _RF_PROTOCOL_H
#define _RF_PROTOCOL_H

#include <stdlib.h>
#include <stdint.h>
typedef uint8_t byte;

#define PACKET_MAGIC ((byte*)"nSST")
#define VERSION_MAJOR ((byte)0)
#define VERSION_MINOR ((byte)1)

#pragma pack(1)
struct packet {
  byte magic[4]; // PACKET_MAGIC
  byte version[2]; // VERSION_MAJOR VERSION_MINOR
  byte from;
  byte to;
  byte type;
  byte length;
  byte* payload; // free(payload)
  byte checksum;
  byte end; // '\0'
};

extern byte calculate_checksum(struct packet p);

extern struct packet make_packet(byte from, byte to, byte type, byte length, byte* payload);

extern int read_stream_to_packet(struct packet* outp,byte b);

extern int packet_to_bytes(byte** outb,struct packet p); // free(outb)

int make_packet_bytes(byte** outb, byte from, byte to, byte type, byte length, byte* payload){ // free(outb)
  struct packet p=make_packet(from, to, type, length, payload);
  int len=packet_to_bytes(outb, p);
  free((void*)p.payload);
  return len;
}

#endif //_RF_PROTOCOL_H
