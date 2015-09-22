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
    cout << "Attempting to connect... " << endl;
    if ((fd = network::connect("45.55.185.4", SERVICE_PORT)) < 0) {
        cout << "Connection failed." << endl;
        return -1;
    } else {
        cout << "Connection succeeded at " << fd << endl;
    }
    try {
        network::requestRegistration(fd);
        registeredId = network::registerId(fd);
        authenticationToken = network::requestAuthentication(fd);
        sessionToken = network::requestChallenge(fd, authenticationToken, registeredId);
        network::requestObject(fd, sessionToken, registeredId);
    } catch (NETWORK_EXC_CODES &exc) {
        printf("Exception: %d\n", exc);
        switch (exc) {
            case 0:
                printf("Request registration exc caught\n");
                break;
            case 1:
                printf("Register ID exc caught\n");
                break;
            case 2:
                printf("Request authentication exc caught\n");
                break;
            case 3:
                printf("Request challenge exc 1 caught\n");
                break;
            case 4:
                printf("Request challenge exc 2 caught\n");
                break;
            case 5:
                printf("Request object exc caught\n");
                break;
        }
    }
    network::close_socket(fd);
    return 0;
}
