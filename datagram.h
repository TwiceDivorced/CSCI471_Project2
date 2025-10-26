//
// Created by Phillip Romig on 7/16/24.
//

#ifndef RFT_DATAGRAM_H
#define RFT_DATAGRAM_H
#include <cstdint>
#include <string>

#define MAX_PAYLOAD_LENGTH 255
struct datagramS {
    uint16_t seqNum = 0;
    uint16_t ackNum = 0;
    uint16_t checksum = 0;
    uint8_t payloadLength = MAX_PAYLOAD_LENGTH;
    char data[MAX_PAYLOAD_LENGTH]{};
};

std::string toString(const struct datagramS &datagram);
uint16_t computeChecksum(const struct datagramS &datagram);
bool validateChecksum(const struct datagramS &datagram);


#endif //RFT_DATAGRAM_H
