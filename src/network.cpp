#include "network.hpp"

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>

namespace network {

	int connect(const std::string& host, const int& port) {
		int socket_fd = socket(AF_INET, SOCK_STREAM, 0);

		if(socket_fd < 0) return -1;

		struct sockaddr_in address;
		address.sin_family = AF_INET;
		address.sin_port = htons(port);
		address.sin_addr.s_addr	 = inet_addr(host.c_str());
		bzero(&(address.sin_zero), 8);

		int connection;
		connection = connect(socket_fd, (struct sockaddr*) &address, sizeof(address));

		if(connection < 0) return -2;

		return socket_fd;
	}

	void write(int fd, const array::array *content) {
		if(fd >= 0) {
			send(fd, content->data, content->length, 0);
		}
		else {
			// TODO throw an exception here.
		}
	}

	array::array* read(int fd) {
		if(fd >= 0) {
			byte* buffer = new byte[RECV_BUFFER_SIZE];
			int read = recv(fd, buffer, RECV_BUFFER_SIZE, 0);
			if(read < 0) return nullptr;

			array::array* msg = array::create(read, buffer);

			delete[] buffer;

			return msg;
		}
		else {
			return nullptr;
			// TODO throw an exception here.
		}
	}

	array::array* read(int fd, size_t len) {
		if(fd >= 0) {
			byte* buffer = new byte[len];
			size_t total_read = 0;

			while(total_read < len) {
				int read = recv(fd, buffer + total_read, len - total_read, 0);
				total_read += read;
			}

			array::array* msg = array::create(total_read, buffer);
			delete[] buffer;
			return msg;
		}
		else return nullptr;
	}

	void close(int fd) {
		if(fd >= 0) {
			close(fd);
		}

		else {
			// TODO throw an exception here.
		}
	}

	void sendPacket(const int fd, Packet *packet) {
	    array::array *rawPacket;
	    size_t packetSize = packet->total_size();
	    rawPacket = array::create(packetSize + 4);
	    byte header[4] = {packetSize & 0xFF, (packetSize >> 8) & 0xFF, (packetSize >> 16 & 0xFF, (packetSize >> 24) & 0xFF)};

	    memcpy(rawPacket->data, header, 4);
	    memcpy(rawPacket->data + 4, packet->bytes()->data, packetSize);

	    printf("Sending packet with length %lu\n", packetSize);
	    for (size_t i = 0; i < packetSize + 4; i++) {
	        printf("%X ", rawPacket->data[i]);
	    }
	    network::write(fd, rawPacket);
	    array::destroy(rawPacket);
	    printf("\n");
	}

	Packet *readPacket(const int fd) {
	    array::array* data = network::read(fd);
	    if (data == nullptr) {
	        printf("Read NULL data.\n");
	    }
		array::array *dataWithoutHeader = array::create(data->length - 4);
		memcpy(dataWithoutHeader->data, data->data + 4, data->length - 4);
		Packet *packet = new Packet(dataWithoutHeader);

		array::destroy(data);
		array::destroy(dataWithoutHeader);
	    return packet;
	}

	void requestAuthentication(int fd) {
		printf("Requesting authentication...\n");
		Packet *packet = new Packet(0xC0);
		network::sendPacket(fd, packet);
	    delete packet;

		packet = readPacket(fd);
		if (packet->tagIs(0xC1)) {
			printf("Authentication complete.\n");
		}
	}
}
