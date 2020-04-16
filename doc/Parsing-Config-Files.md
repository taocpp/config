# Parsing Config Files

*Warning: The documentation is still work-in-progress and very incomplete.*

*  [Reading](#reading)
*  [Inspecting](#inspecting)
*  [Annotations](#annotations)

Requires good C++17 support, currently tested on GCC 8 and 9 and "recent" Clang.

## Reading

If you manage to compile the tests and examples by tweaking the `Makefile`, and want to try this out in your application, simply include `tao/config.hpp` and use one of the functions

```c++
tao::config:value tao::config::parse_file( const std::string& file );
tao::config:value tao::config::parse_files( const std::vector< std::string >& files );
```

to obtain a `tao::config::value`, which is a `tao::json::basic_value< tao::config::traits >`.

## Inspecting

The usual [JSON Value API](https://github.com/taocpp/json) can be used to inspect and operate on the obtained config.

In addition, some config library specific functions are available that operate on *config keys*.

A config key is an instance of `tao::config::key` which consists mostly of a `std::vector< tao::config::part >`.

## Annotations

The returned [JSON Values](https://github.com/taocpp/json) use the JSON library's annotation feature to annotate the values with a config Key, and the filename and position they were parsed from.

The config Key is a kind of strongly typed JSON Pointer; unlike JSON Pointer, the string "2" and the integer 2 are distinguished, and strings can only be used to index objects, while integers can only be used to index arrays.

Parsing multiple files with `parse_files()` does the same as having each config file include the next one in the list at the end, or the same as having a single additional top-level config file include all others in the correct order.

Alternative versions of these functions take a template parameter for a `tao::json::basic_value<>` with different traits as result.

```c++
template< template< typename... > class Traits >
tao::json::basic_value< Traits > tao::config::basic_parse_file( const std::string& file );
template< template< typename... > class Traits >
tao::json::basic_value< Traits > tao::config::basic_parse_files( const std::vector< std::string >& files );
```

The pointer and position information is given to each result (sub-)value via the public base class' `set_pointer()` and `set_position()` methods, see `tao/config/annotation.hpp`.
These methods are individually optional, they will only be called with the pointer and/or position information, respectively, when present.

For further details please Use The Source, or contact us if you have any questions.

Copyright (c) 2018-2020 Dr. Colin Hirsch and Daniel Frey
