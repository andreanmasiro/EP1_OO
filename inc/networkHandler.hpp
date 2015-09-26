#ifndef NETWORK_HANDLER_H
#define NETWORK_HANDLER_H

#include "array.hpp"

#define ECHO_PORT 3001
#define SERVICE_PORT 3000

class NetworkHandler {
    private:
        int fd;
        array::array *registeredId;
        array::array *authenticationToken;
        array::array *sessionToken;
        array::array *obj;
    public:
        NetworkHandler();
        void connect();
        void registerProtocol();
        void authenticationProtocol();
        void requestProtocol();
        array::array *getObject();
        void endConnection();
};

#endif
