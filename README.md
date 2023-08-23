# Clang AST Filter

Quick-and-dirty utility for dumping a Clang AST to JSON with system headers filtered out.

Requires `clang` and [json](https://github.com/nlohmann/json)

Make sure that `nlohmann/json.hpp` is in your include path.

Usage: 
```
make
./ast-filter <file-to-dump>
```