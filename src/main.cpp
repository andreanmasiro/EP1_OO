#include <iostream>
#include <cstring>
#include "network.hpp"
#include "crypto.hpp"
#include "array.hpp"
#include "packet.hpp"

using namespace std;

void sendPacket(const int fd, Packet *packet) {
    array::array *rawPacket;
    size_t packetSize = packet->total_size();
    rawPacket = array::create(packetSize + 4);
    byte header[4] = {packetSize & 0xFF, (packetSize >> 8) & 0xFF, (packetSize >> 16 & 0xFF, (packetSize >> 24) & 0xFF)};

    memcpy(rawPacket->data, header, 4);
    memcpy(rawPacket->data + 4, packet->bytes()->data, packetSize);

    printf("Sending packet with length %d\n", packetSize);
    for (size_t i = 0; i < packetSize + 4; i++) {
        printf("%X ", rawPacket->data[i]);
    }
    network::write(fd, rawPacket);
    array::destroy(rawPacket);
    printf("\n");
}

array::array *readPacket(const int fd) {
    array::array* data = network::read(fd);
    if (data == nullptr) {
        printf("Read NULL data.\n");
    }
    return data;
}

int main() {
    int fd;
    if ((fd = network::connect("45.55.185.4", 3001)) < 0) {
        cout << "Connection failed." << endl;
        return -1;
    } else {
        cout << "Connection succeeded at " << fd << endl;
    }

    byte valueBytes[4] = {0xFF, 0x24, 0x12, 0x35};
    array::array *value = array::create(sizeof(byte)*4);
    memcpy(value->data, valueBytes, sizeof(byte)*4);


    Packet *packet = new Packet(0xC0, value);

    // array::destroy(value);
    sendPacket(fd, packet);
    // array::array *packet = array::create(7);
    // byte packetSize[4] = {0x03, 0x0, 0x0, 0x0};
    // memcpy(packet->data, packetSize, 4);
    //
    // byte packetInfo[3] = {0xC0, 0x0, 0x0};
    // memcpy(packet->data + 4, packetInfo, 3);
    //
    // network::write(fd, packet);
    array::array *received_packet;
    received_packet = readPacket(fd);
    if (received_packet != nullptr) {
        cout << "Receiving packet with length: " << received_packet->length << endl;
        for (size_t i = 0; i < received_packet->length; i++) {
            printf("%X ", received_packet->data[i]);
        }
    }
    printf("\n");

    return 0;
}
