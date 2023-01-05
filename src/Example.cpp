#include <Example.hpp>
#include <iostream>

Example::Example(/*Args*/) : value(5) {
	std::cout << "creating" << std::endl;
}
Example::Example(Example const & src) {
	*this = src;
}

Example::~Example(void) {
	std::cout << "deleting " << this->value << std::endl;
}

Example & Example::operator=(Example const & rhs) {
	this->value = rhs.value;
	return (*this);
}
