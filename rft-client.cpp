//
// Created by Phillip Romig on 7/16/24.
// Finished off by Stewart Erhardt on 10/25/24.
//
#include <iostream>
#include <fstream>
#include <sys/socket.h>
#include <system_error>
#include <unistd.h>
#include <array>

#include "timerC.h"
#include "unreliableTransport.h"
#include "logging.h"

#define WINDOW_SIZE 10
#define TIMEOUT_MS 500
int main(int argc, char *argv[])
{

    // Defaults
    uint16_t portNum(12345);
    std::string hostname("");
    std::string inputFilename("");
    int requiredArgumentCount(0);

    int opt;
    try
    {
        while ((opt = getopt(argc, argv, "f:h:p:d:")) != -1)
        {
            switch (opt)
            {
            case 'p':
                portNum = std::stoi(optarg);
                break;
            case 'h':
                hostname = optarg;
                requiredArgumentCount++;
                break;
            case 'd':
                LOG_LEVEL = std::stoi(optarg);
                break;
            case 'f':
                inputFilename = optarg;
                requiredArgumentCount++;
                break;
            case '?':
            default:
                std::cout << "Usage: " << argv[0] << " -f filename -h hostname [-p port] [-d debug_level]" << std::endl;
                break;
            }
        }
    }
    catch (std::exception &e)
    {
        std::cout << "Usage: " << argv[0] << " -f filename -h hostname [-p port] [-d debug_level]" << std::endl;
        FATAL << "Invalid command line arguments: " << e.what() << ENDL;
        return (-1);
    }

    if (requiredArgumentCount != 2)
    {
        std::cout << "Usage: " << argv[0] << " -f filename -h hostname [-p port] [-d debug_level]" << std::endl;
        std::cerr << "hostname and filename are required." << std::endl;
        return (-1);
    }

    TRACE << "Command line arguments parsed." << ENDL;
    TRACE << "\tServername: " << hostname << ENDL;
    TRACE << "\tPort number: " << portNum << ENDL;
    TRACE << "\tDebug Level: " << LOG_LEVEL << ENDL;
    TRACE << "\tOutput file name: " << inputFilename << ENDL;

    // *********************************
    // * Open the input file
    // *********************************

    std::ifstream inputFile;
    inputFile.open(inputFilename, std::ios::in | std::ios::binary);
    if (!inputFile.is_open())
    {
        FATAL << "Unable to open input file: " << inputFilename << ENDL;
        return (-1);
    }
    TRACE << "Input file opened: " << inputFilename << ENDL;

    try
    {

        // ***************************************************************
        // * Initialize your timer, window and the unreliableTransport etc.
        // **************************************************************
        auto *network = new unreliableTransportC(hostname, portNum);
        timerC timer;
        std::array<datagramS, WINDOW_SIZE> window{};

        // ***************************************************************
        // * Send the file one datagram at a time until they have all been
        // * acknowledged
        // **************************************************************
        bool allSent(false);
        bool allAcked(false);
        int base(1);
        int nextSeqNum(1);
        while ((!allSent) && (!allAcked))
        {

            // Is there space in the window? If so, read some data from the file and send it.
            while ((nextSeqNum < base + WINDOW_SIZE) && (!allSent))
            {
                datagramS &datagram = window[nextSeqNum % WINDOW_SIZE];
                inputFile.read(datagram.data, MAX_PAYLOAD_LENGTH);
                std::streamsize bytesRead = inputFile.gcount();
                if (bytesRead == 0)
                {
                    // send a zero-length datagram as an EOF
                    datagram.seqNum = nextSeqNum;
                    datagram.payloadLength = 0;
                    datagram.checksum = computeChecksum(datagram);
                    network->udt_send(datagram);
                    TRACE << "Sent final EOF datagram with seqNum " << datagram.seqNum << ENDL;

                    allSent = true;
                    break;
                }

                // Prepare and send datagram
                datagram.seqNum = nextSeqNum;
                datagram.payloadLength = static_cast<uint8_t>(bytesRead);
                datagram.checksum = computeChecksum(datagram);
                network->udt_send(datagram);
                TRACE << "Sent datagram with seqNum " << datagram.seqNum << " and payloadLength " << static_cast<int>(datagram.payloadLength) << ENDL;
                if (base == nextSeqNum)
                {
                    timer.setDuration(TIMEOUT_MS);
                    timer.start();
                }
                nextSeqNum++;
            }

            // Call udt_receive() to see if there is an acknowledgment.  If there is, process it.

            datagramS ackDatagram{};
            ssize_t recvBytes = network->udt_receive(ackDatagram);
            if (recvBytes > 0)
            {
                // Validate checksum before trusting the ACK
                if (!validateChecksum(ackDatagram))
                {
                    WARNING << "Bad checksum, ignoring." << ENDL;
                }
                else
                {
                    TRACE << "Received ACK for seqNum " << ackDatagram.ackNum << ENDL;
                    if ((ackDatagram.ackNum >= base) && (ackDatagram.ackNum < nextSeqNum))
                    {
                        base = ackDatagram.ackNum + 1;
                        if (base == nextSeqNum)
                        {
                            timer.stop();
                        }
                        else
                        {
                            timer.start();
                        }
                    }
                    else if (ackDatagram.ackNum < base)
                    {
                        // duplicate/old ACK
                        DEBUG << "Received duplicate/old ACK for " << ackDatagram.ackNum << " (base=" << base << ")" << ENDL;
                    }
                    else
                    {
                        // ACK for future
                        WARNING << "Received ACK for seqNum " << ackDatagram.ackNum << " which is >= nextSeqNum (" << nextSeqNum << "). Ignoring." << ENDL;
                    }
                }
            }

            // Check to see if the timer has expired.

            if (timer.timeout())
            {
                TRACE << "Timeout occurred. Retransmitting datagrams from seqNum " << base << ENDL;
                // Retransmit all unacknowledged datagrams in the window
                for (int i = base; i < nextSeqNum; ++i)
                {
                    datagramS &datagram = window[i % WINDOW_SIZE];
                    network->udt_send(datagram);
                    TRACE << "Resent datagram with seqNum " << datagram.seqNum << ENDL;
                }
                timer.start(); // Restart the timer
            }

            // Check if all datagrams have been acknowledged
            if (allSent && (base == nextSeqNum))
            {
                allAcked = true;
                TRACE << "All datagrams acknowledged." << ENDL;
            }
        }

        // cleanup and close the file and network.
        delete network;
        inputFile.close();
        TRACE << "File transfer complete. Exiting." << ENDL;
    }
    catch (std::exception &e)
    {
        FATAL << "Error: " << e.what() << ENDL;
        exit(1);
    }
    return 0;
}
