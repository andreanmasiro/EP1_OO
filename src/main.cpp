#include <iostream>
#include <cstring>
#include "network.hpp"
#include "networkHandler.hpp"
#include "crypto.hpp"
#include "array.hpp"
#include "packet.hpp"

using namespace std;

int main() {

    NetworkHandler *nh = new NetworkHandler();
    nh->connect();
    try {
        nh->registerProtocol();
        nh->authenticationProtocol();
        nh->requestProtocol();

        FILE *fp = fopen("object.png", "wb");
        array::write_array(fp, nh->getObject());
        fclose(fp);

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

    return 0;
}
