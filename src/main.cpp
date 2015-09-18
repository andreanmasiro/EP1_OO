#include <iostream>
#include "network.hpp"
#include "crypto.hpp"
#include "array.hpp"

using namespace std;

int main() {
    int fd;
    if ((fd = network::connect("45.55.185.4", 3001)) < 0) {
        cout << "Connection failed." << endl;
        return -1;
    } else {
        cout << "Connection succeeded at " << fd << endl;
    }

    array::array *packet = array::create(7);
    byte packetSize[4] = {0x03, 0x0, 0x0, 0x0};
    memcpy(packet->data, packetSize, 4);

    byte packetInfo[3] = {0xC0, 0x0, 0x0};
    memcpy(packet->data + 4, packetInfo, 3);

    network::write(fd, packet);
    array::array *received_packet;
    received_packet = network::read(fd);
    if (received_packet != nullptr) {
        cout << "Packet length: " << received_packet->length << endl;
        for (size_t i = 0; i < received_packet->length; i++) {
            printf("0x%X ", received_packet->data[i]);
        }
    }
    printf("\n");

    return 0;
}
