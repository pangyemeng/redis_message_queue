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
		int age;
		std::string name;
		MSGPACK_DEFINE(age, name);
}Item;


int main()
{
	Item data;
	data.age = 28;
	data.name = "pym";

    std::stringstream buffer;
    msgpack::pack(buffer, data);

    // send the buffer ...
    buffer.seekg(0);

    // deserialize the buffer into msgpack::object instance.
	std::string str(buffer.str());

	msgpack::object_handle oh = msgpack::unpack(str.data(), str.size());

	// deserialized object is valid during the msgpack::object_handle instance alive.
	msgpack::object deserialized = oh.get();

	// msgpack::object supports ostream.
	std::cout << deserialized << std::endl;

	// convert msgpack::object instance into the original type.
	// if the type is mismatched, it throws msgpack::type_error exception.
	Item dst;
	deserialized.convert(dst);

	printf("name: %s\nage : %d\n", dst.name.c_str(), dst.age);
	return 0;
}
