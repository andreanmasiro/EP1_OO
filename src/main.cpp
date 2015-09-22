#include <iostream>
#include <cstring>
#include "network.hpp"
#include "crypto.hpp"
#include "array.hpp"
#include "packet.hpp"

#define ECHO_PORT 3001
#define SERVICE_PORT 3000

using namespace std;

int main() {
    int fd;
    array::array *registeredId;
    array::array *authenticationToken;
    array::array *sessionToken;
    if ((fd = network::connect("45.55.185.4", SERVICE_PORT)) < 0) {
        cout << "Connection failed." << endl;
        return -1;
    } else {
        cout << "Connection succeeded at " << fd << endl;
    }
    network::requestRegistration(fd);
    registeredId = network::registerId(fd);
    authenticationToken = network::requestAuthentication(fd);
    sessionToken = network::requestChallenge(fd, authenticationToken, registeredId);

    network::requestObject(fd, sessionToken, registeredId);
    network::close_socket(fd);
    return 0;
}
