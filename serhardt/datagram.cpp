//
// Created by Phillip Romig on 7/16/24.
//

#include "datagram.h"

std::string toString(const datagramS &datagram) {
    std::string str;
    str = "seqNum: " + std::to_string(datagram.seqNum);
    str += " ackNum: " + std::to_string(datagram.ackNum);
    str += " payloadLength: " + std::to_string(datagram.payloadLength);
    str += " computeChecksum: " + std::to_string(datagram.checksum) ;
    str += " data: ";
    for (char i : datagram.data)
        if (i != '\0')
            str += i;
    return str;
}

uint16_t computeChecksum(const struct datagramS &datagram) {
    uint16_t sum = 0;
    sum += datagram.seqNum;
    sum += datagram.ackNum;
    sum += datagram.payloadLength;
    for (int i=0; i<datagram.payloadLength; i++)
        sum += datagram.data[i];
    return sum;
}

bool validateChecksum(const datagramS &datagram) {
    return computeChecksum(datagram) == datagram.checksum;
}