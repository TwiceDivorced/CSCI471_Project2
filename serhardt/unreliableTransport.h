//
// Created by Phillip Romig on 7/15/24.
//

#ifndef UNRELIABLE_TRANSPORT_H
#define UNRELIABLE_TRANSPORT_H

#include <netinet/in.h>
#include "datagram.h"

class unreliableTransportC {
private:
    int socketFd;
    struct sockaddr_in serverAddr{};
public:
   unreliableTransportC() : socketFd(0) {};
   explicit unreliableTransportC(const std::string &hostname, const uint16_t &portNum);
   ~unreliableTransportC();
   void udt_send(const datagramS &data) const;
   [[nodiscard]] ssize_t udt_receive(const datagramS &data) const ;
};

#endif // UNRELIABLE_TRANSPORT_H
