#ifndef _NETWORK_HPP
#define _NETWORK_HPP

#include "array.hpp"
#include "packet.hpp"

#include <string>

#define RECV_BUFFER_SIZE 1024 * 1024

typedef enum {
    REGISTRATION_START_EXC_CODE = 0,
    REGISTER_ID_EXC_CODE,
    AUTH_EXC_CODE,
    REQ_CHALLENGE_EXC_CODE_1,
    REQ_CHALLENGE_EXC_CODE_2,
    REQ_OBJECT_EXC_CODE
} NETWORK_EXC_CODES;

namespace network {
    
    /*
     Connects to a remote host and returns a file descriptor representing
     the socket. A negative value means that the connection failed.
     */
    int connect(const std::string& host, const int& port);
    
    /*
     Closes a socket.
     */
    void close_socket(int fd);
    
    /*
     Writes the data in the _content_ array into the giving socket.
     */
    void write(int fd, const array::array* content);
    
    /*
     Reads data from the socket specified in _fd_ and returns an array containing
     this data.
     */
    array::array* read(int fd);
    
    /*
     Reads len bytes of data from the socket specified in _fd_ and returns an
     array containing this data.
     */
    array::array* read(int fd, size_t len);
    
    
    void sendPacket(const int fd, Packet *packet);
    
    Packet *readPacket(const int fd);
    
    void requestRegistration(int fd);
    
    array::array *registerId(int fd);
    
    array::array *requestAuthentication(int fd);
    
    array::array *requestChallenge(int fd, array::array *iv, array::array *key);
    
    void requestObject(int fd, array::array *iv, array::array *key);
}
#endif
