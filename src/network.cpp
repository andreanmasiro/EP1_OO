#include "network.hpp"

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>

#define USER_ID {0x5C, 0x47, 0x69, 0x89, 0x60, 0x16, 0x8E, 0x67}
#define OBJ_ID {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08}

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

    void close_socket(int fd) {
        if(fd >= 0) {
            close(fd);
            printf("Socket closed.\n");
        }
        else {
            // TODO throw an exception here.
        }
    }

    void sendPacket(const int fd, Packet *packet) {
        array::array *rawPacket;
        size_t packetSize = packet->total_size();
        rawPacket = array::create(packetSize + 4);
        byte header[4] = {packetSize & 0xFF, (packetSize >> 8) & 0xFF, (packetSize >> 16) & 0xFF, (packetSize >> 24) & 0xFF};

        memcpy(rawPacket->data, header, 4);
        memcpy(rawPacket->data + 4, packet->bytes()->data, packetSize);

        network::write(fd, rawPacket);
        array::destroy(rawPacket);
        printf("\n");
    }

    Packet *readPacket(const int fd) {
        array::array *header = network::read(fd, 4);
        array::array *data = nullptr;
        if (header == nullptr) {
            printf("Read NULL data.\n");
        } else {
            size_t dataSize = header->data[0] | (header->data[1] << 8) | (header->data[2] << 16) | (header->data[3] << 24);
            data = network::read(fd, dataSize);
        }

        Packet *packet = new Packet(data);
        printf("Read packet size = %d\n", data->length);
        array::destroy(data);

        return packet;
    }

    void requestRegistration(int fd) {
        printf("Requesting registration...\n");
        Packet *packet = new Packet(0xC0);
        sendPacket(fd, packet);
        delete packet;

        packet = readPacket(fd);
        if (packet->tagIs(0xC1)) {
            printf("Registration complete.\n");
        } else {
            throw(REGISTRATION_START_EXC_CODE);
        }
    }

    array::array * registerId(int fd) {
        printf("Registering user ID...\n");
        RSA *key = crypto::rsa_read_public_key_from_PEM("server_pk.pem");
        byte userId[8] = USER_ID;

        array::array *id = array::create(8, userId);
        array::array *enc_id = crypto::rsa_encrypt(id, key);
        array::destroy(id);

        Packet *packet = new Packet(0xC2, enc_id);
        sendPacket(fd, packet);
        delete packet;

        packet = readPacket(fd);
        if (packet->tagIs(0xC3)) {
            /* code */
            array::array *s_enc = packet->getValue();

            crypto::rsa_destroy_key(key);
            key = crypto::rsa_read_private_key_from_PEM("private.pem");

            array::array *s_dec = crypto::rsa_decrypt(s_enc, key);
            printf("User registered.\n");
            return s_dec;
        } else {
            throw(REGISTER_ID_EXC_CODE);
        }
    }

    array::array *requestAuthentication(int fd) {
        printf("Requesting authentication...\n");

        RSA *key = crypto::rsa_read_public_key_from_PEM("server_pk.pem");
        byte userId[8] = USER_ID;

        array::array *id = array::create(8, userId);
        array::array *enc_id = crypto::rsa_encrypt(id, key);
        array::destroy(id);

        Packet *packet = new Packet(0xA0, enc_id);
        sendPacket(fd, packet);
        delete packet;

        packet = readPacket(fd);
        if (packet->tagIs(0xA1)) {
            /* code */
            array::array *a_enc = packet->getValue();

            crypto::rsa_destroy_key(key);
            key = crypto::rsa_read_private_key_from_PEM("private.pem");

            array::array *a_dec = crypto::rsa_decrypt(a_enc, key);
            printf("Authentication started.\n");
            return a_dec;
        } else {
            throw(AUTH_EXC_CODE);
        }
    }

    array::array *requestChallenge(int fd, array::array *iv, array::array *key) {
        printf("Requesting challenge...\n");
        Packet *packet = new Packet(0xA2);
        sendPacket(fd, packet);
        delete packet;

        packet = readPacket(fd);
        printf("Challenge accepted.\n");
        if (packet->tagIs(0xA4)) {
            array::array *decrypted = crypto::aes_decrypt(packet->getValue(), iv, key);
            printf("Challenge decrypted.\n");
            delete packet;
            packet = new Packet(0xA5, decrypted);
            sendPacket(fd, packet);
            delete packet;

            packet = readPacket(fd);
            if (packet->tagIs(0xA6)) {
                printf("Authenticated\n");
                return crypto::aes_decrypt(packet->getValue(), iv, key);
            }
            printf("Challenge decrypted incorrectly.\n");
            throw(REQ_CHALLENGE_EXC_CODE_1);
        }
        printf("Challenge not solved correctly.\n");
        throw(REQ_CHALLENGE_EXC_CODE_2);
    }

    array::array *requestObject(int fd, array::array *iv, array::array *key) {
        printf("Requesting object...\n");
        byte objId[8] = OBJ_ID;
        array::array *id = array::create(8, objId);
        array::array *c_id = crypto::aes_encrypt(id, iv, key);

        Packet *packet = new Packet(0xB0, c_id);
        sendPacket(fd, packet);
        delete packet;
        array::destroy(id);
        array::destroy(c_id);

        packet = readPacket(fd);

        if (packet->tagIs(0xB1)) {
            printf("AEEE\n");
            array::array *obj = crypto::aes_decrypt(packet->getValue(), iv, key);
            return obj;
        } else {
            throw(REQ_OBJECT_EXC_CODE);
        }
    }
}
