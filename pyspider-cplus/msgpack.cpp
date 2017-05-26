/*
 * msgpack.cpp
 *
 *  Created on: 2017Äê5ÔÂ24ÈÕ
 *      Author: Administrator
 */
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <iostream>
#include <sstream>
#include <msgpack.hpp>

typedef struct item
{
		int num;
		std::string name;
		//char *data
		msgpack::type::raw_ref  data;
		MSGPACK_DEFINE(num, name, data);
} Item;

void print(std::string const& buf) {
    for (std::string::const_iterator it = buf.begin(), end = buf.end();
         it != end;
         ++it) {
        std::cout
            << std::setw(2)
            << std::hex
            << std::setfill('0')
            << (static_cast<int>(*it) & 0xff)
            << ' ';
    }
    std::cout << std::dec << std::endl;
}

int main()
{
	Item data;
	char *str_test = "test";

	data.num = 28;
	data.name = "pym";
	data.data.ptr = str_test;
	data.data.size = strlen(str_test) + 1;

    std::stringstream buffer;
    msgpack::pack(buffer, data);

    // send the buffer ...
    buffer.seekg(0);


    // deserialize the buffer into msgpack::object instance.
	std::string str(buffer.str());
	print(str);
	msgpack::object_handle oh = msgpack::unpack(str.data(), str.size());

	// deserialized object is valid during the msgpack::object_handle instance alive.
	msgpack::object deserialized = oh.get();

	// msgpack::object supports ostream.
	std::cout << deserialized << std::endl;

	// convert msgpack::object instance into the original type.
	// if the type is mismatched, it throws msgpack::type_error exception.
	Item dst;
	deserialized.convert(dst);

	printf("name: %s\nnum : %d\n", dst.name.c_str(), dst.num);
	return 0;
}
