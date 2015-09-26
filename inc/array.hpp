#ifndef _ARRAY_H
#define _ARRAY_H

#include <cstring>
#include <string>
#include <ostream>

#define WRAPPED_ARRAY 1
#define NORMAL_ARRAY 2

typedef unsigned char byte;

namespace array {

    typedef struct _array {
        byte *data ;
        size_t length;
        byte _tag;
    } array;

    std::ostream& operator<<(std::ostream &os, const array& a);
    /*
     Creates a new array with the given capacity
     */
    array* create(const size_t& capacity);

    /*
     Creates a new array with the given capacity and copies the bytes from data
     to the new created array.
     */
    array* create(const size_t& capacity, byte* data);

    /*
     Creates a new array with the given capacity and points the data to the given
     pointer.
     */
    array* wrap(const size_t& capacity, byte* data);


    /*
     Destroy an array. If this is a wrapped array it will not free its data.
     */
    void destroy(array* a);

    /*
     Copies an array. If the array is wrapped, it will not copy all the bytes
     from its data but instead it will just point to it.
     */
    array* copy(const array* a);

    void print_array(array *array);

    void write_array(FILE *fp, array *array);
}
#endif
