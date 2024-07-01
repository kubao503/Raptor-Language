#include "types.hpp"

ReturnType typeToReturnType(const Type& type) {
    return std::visit([](auto t) -> ReturnType { return t; }, type);
}