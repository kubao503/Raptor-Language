### Prerequisites
- conan 2.2.1
- cmake 3.16.3
- lcov 1.14
- genhtml 1.14
- doxygen 1.10.0

All of them can be installed using setup.sh

### Installation:

```console
$ conan install . --build=missing -s compiler.cppstd=23 -s build_type=Debug
$ cd build/Debug/
$ cmake ../../ -DCMAKE_TOOLCHAIN_FILE=generators/conan_toolchain.cmake -DCMAKE_BUILD_TYPE=Debug
$ cmake --build .
```

### Running tests:

```console
$ cd build/Debug/
$ ctest
```

### Running interpreter:

```console
$ cd build/Release
$ ./src/interpreter/interpreter_program ../../example.rp
```

### Running compiler:

```console
$ cd build/Release
$ ./src/compiler/compiler_program ../../example.rp 
$ g++ output.o src/compiler/CMakeFiles/print.dir/print.cpp.o -o prog
$ ./prog
```

### Getting test coverage

```console
$ cd build/Debug/
$ lcov --zerocounters --directory .
$ ctest
$ lcov --capture --directory . --output-file coverage.info
$ genhtml coverage.info --output-directory coverage_report
```
Coverage report is then accessible at build/Debug/coverage_report/index.html

### Generating documentation
```console
$ doxygen
```
Documentation is then accessible at html/index.html

### Code formatting
.clang-format file + clang-format built into VSCode

### Code linting
clang-tidy built into VSCode