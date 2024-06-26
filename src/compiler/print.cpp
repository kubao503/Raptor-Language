#include <iostream>

extern "C" void printValue(int value) {
    std::cout << value << std::endl;
}