#ifndef PRINT_H
#define PRINT_H

enum class TypeIndex { INT, FLOAT, BOOL, STR, VOID };

extern "C" void printValue(int type, ...);

#endif  // PRINT_H