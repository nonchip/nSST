# Protocol specs

we'll use a custom protocol on top of UART `8bit, 1par, >=2stop`.

Baudrate of the UART should be either user specified or (for duplex connections only) autoconfigured by starting with 300 and then performing a `type04` handshake.

## Packet format

            +---+---+---+---+----+----+------+----+------+--------+----------+------------+------------+
    Offset  | 0 | 1 | 2 | 3 |  4 |  5 |   6  |  7 |   8  |    9   |    A…    | B+(Length) | C+(Length) |
            +---+---+---+---+----+----+------+----+------+--------+----------+------------+------------+
    Field   |    Magic      | Version | From | To | Type | Length | Payload  |  Checksum  | End Marker |
            +---------------+---------+------+----+------+--------+----------+------------+------------+
    Size    |       4       |    2    |   1  |  1 |   1  |    1   | (Length) |     1      |      1     |
            +---------------+---------+------+----+------+--------+----------+------------+------------+
    Data    |     "nSST"    |  00 00  |   …  |  … |   …  |    …   |     …    |     …      |     00     |
            +---------------+---------+------+----+------+--------+----------+------------+------------+

Fields:

 * Magic
   4 Bytes MUST BE `ASCII"nSST" (0x6E 0x53 0x53 0x54)` 
 * Version
   2 Bytes MUST BE `0.0 (0x00 0x00)` as of this spec version
   when parsing a message with the correct "first" (=major) version byte it should be considered compatible while the second byte is for minor changes or feature flags.
 * From
   1 Byte packet source address (if sending device can't receive answer packets, set this to 0)
 * To
   1 Byte packet destination address (all devices should listen on their selected address and additionally on 0=broadcast if some setting/jumper/etc is set)
 * Type
   1 Byte packet type
 * Length
   1 Byte payload length (NOT packet length)
 * Payload
   (Length) Bytes packet payload
 * Checksum
   1 Byte sum of all previous bytes in the packet (wrapping overflow)
 * End Marker
   1 Byte MUST BE `0x00`

### Packet types

#### 01: Heartbeat
e.g. for out-of-range detection, Ping

    Type: 01
    Length: 01
    Payload Bits:
      0 1 2 3 4 5 6 7
      | | | | | | | |
      | | | | | | | +-- 0: Ping/Pong, 1: Heartbeat
      | | | | | | +---- If Ping/Pong (7=0):
      | | | | | |         0: Ping request, 1: Pong response
      | | | | | |       If Heartbeat (7=1):
      | | | | | |         should invert each time.
      | | | | | +------ Receive flag: 0: device can NOT receive, 1: device CAN receive
      | | | | |
      +-+-+-+-+-------- RESERVED (just 0 for now)

#### 02: digital flags
e.g. switch outputs on/off, enable/disable operation modes, digital sensors, etc

    Type: 02
    Length: 01..FF
    Payload Bits:
      0 1 …
      | | |
      | | +-- the actual flags (might be multiple bytes long)
      +-+---- mode:
                00: AND
                01: OR
                10: XOR
                11: SET

#### 03: analog value
e.g. intensity, analog sensors, etc

    Type: 03
    Length: 02
    Payload Bytes:
      0 1
      | |
      | +-- set to value
      +---- slot number

#### 04: change baudrate

    Type: 04
    Length: 01
    Payload Bits:
      0 1...
      | |
      | +-- new baudrate divided 300
      +---- 0: offer new baudrate, 1: accept new baudrate (send 3 times, then change rate; change rate if received; then ping and fallback to 300 on error)

