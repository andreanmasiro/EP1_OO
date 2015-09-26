#include "networkHandler.hpp"
#include "network.hpp"

NetworkHandler::NetworkHandler() {
    this->registeredId = nullptr;
    this->authenticationToken = nullptr;
    this->sessionToken = nullptr;
    this->obj = nullptr;
}

void NetworkHandler::connect() {
    cout << "Attempting to connect... " << endl;
    if ((this->fd = network::connect("45.55.185.4", SERVICE_PORT)) < 0) {
        cout << "Connection failed." << endl;
    } else {
        cout << "Connection succeeded at " << this->fd << endl;
    }
}

void NetworkHandler::registerProtocol() {
    network::requestRegistration(this->fd);
    this->registeredId = network::registerId(this->fd);
}

void NetworkHandler::authenticationProtocol() {
    this->authenticationToken = network::requestAuthentication(this->fd);
    sessionToken = network::requestChallenge(this->fd, this->authenticationToken, this->registeredId);
}

void NetworkHandler::requestProtocol() {
    this->obj = network::requestObject(fd, sessionToken, registeredId);
}

array::array *NetworkHandler::getObject() {
    return this->obj;
}

void NetworkHandler::endConnection() {
    network::close_socket(this->fd);
}
