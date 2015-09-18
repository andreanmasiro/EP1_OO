#include "packet.hpp"
#include "crypto.hpp"

Packet::Packet(const array::array* data) {
    byte* d = data->data;
    this->tag = d[0];
    this->length = d[0] & (d[1] << 8);

    if(this->length > 0) {
        this->value = array::create(this->length, d + 3);
        this->signature = array::create(20, d + this->length + 3);
    }
}

Packet::Packet(const byte tag, const array::array* value)  {
    this->tag = tag;
    this->set_value(value);
}

Packet::Packet(const byte tag) {
    this->tag = tag;
    this->value = nullptr;
    this->signature = nullptr;
}

Packet::~Packet() {
    if(this->value != nullptr) {
        array::destroy(this->value);
        array::destroy(this->signature);
    }
}

size_t Packet::total_size() {
    return 3 + (value != nullptr ? value->length : 0) + (signature != nullptr ? signature->length : 0);
}

void Packet::set_value(const array::array* value) {
    if(this->value != nullptr) {
        array::destroy(this->value);
        array::destroy(this->signature);
    }

    this->value = array::copy(value);
    if(value != nullptr) {
        this->signature = crypto::sha1(value);
        this->length = this->value->length;
    }
    else {
        this->length = 0;
    }
}

array::array* Packet::bytes() {
    array::array* raw;
    if(this->length == 0) {
        raw = array::create(3);
    }
    else {
        raw = array::create(this->total_size());
    }

    raw->data[0] = this->tag;
    raw->data[1] = this->length & 0xFF;
    raw->data[2] = (this->length >> 8) & 0xFF;

    if(this->length > 0) {
        memcpy(raw->data + 3, this->value, this->length);
        memcpy(raw->data + 3 + this->length, this->signature, 20);
    }

    return raw;
}
