### Prerequisites
- conan 2.2.1
- cmake 3.16.3
- lcov 1.14
- genhtml 1.14
- doxygen 1.10.0

### Installation:

```console
$ conan install . --build=missing
$ cd build/Release/
$ cmake ../../ -DCMAKE_TOOLCHAIN_FILE=generators/conan_toolchain.cmake -DCMAKE_BUILD_TYPE=Release
$ cmake --build .
```

### Running tests:

```console
$ cd build/Debug/
$ ctest
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
