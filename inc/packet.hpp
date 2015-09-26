#ifndef PACKET_H
#define PACKET_H

#include <iostream>
#include "array.hpp"
#include "crypto.hpp"

using namespace std;

class Packet;

ostream& operator<<(ostream& os, const Packet& p);

class Packet {
public:
    Packet(const array::array *data);
    Packet(const byte tag, const array::array *value);
    Packet(const byte tag);
    ~Packet();
    size_t total_size();
    void setValue(const array::array *value);
    array::array *getValue();
    array::array *bytes();
    friend ostream& operator<<(ostream &os, const Packet &p);
    int tagIs(byte tag);

private:
    byte tag;
    unsigned short length;
    array::array *value;
    array::array *signature;
};

#endif
