#ifndef OVERLOADED_TPP
#define OVERLOADED_TPP

template <class... Ts>
struct overloaded : Ts... {
    using Ts::operator()...;
};

#endif