#include "print.hpp"

#include <cstdarg>
#include <iostream>

extern "C" void printValue(int type, ...) {
    va_list args;
    va_start(args, type);

    switch (static_cast<TypeIndex>(type)) {
        case TypeIndex::INT: {
            const auto value = va_arg(args, int);
            std::cout << value << '\n';
            break;
        }
        case TypeIndex::FLOAT: {
            const auto value = va_arg(args, double);
            std::cout << value << '\n';
            break;
        }
        case TypeIndex::BOOL: {
            const auto value = va_arg(args, int);
            std::cout << std::boolalpha << static_cast<bool>(value) << '\n';
            break;
        }
        case TypeIndex::STR: {
            char* stringValue = va_arg(args, char*);
            std::cout << stringValue << '\n';
            break;
        }
        case TypeIndex::VOID: {
            std::cout << '\n';
            break;
        }
        default:
            throw std::runtime_error("Unknown type id" + std::to_string(type));
            break;
    }

    va_end(args);
}