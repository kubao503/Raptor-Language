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