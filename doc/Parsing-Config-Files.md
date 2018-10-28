# Parsing Config Files

*Warning: The documentation is still work-in-progress and very incomplete.*

Requires good C++17 support, currently only tested on GCC 8.

If you manage to compile the tests and examples by tweaking the `Makefile`, and want to try this out in your application, simply include `tao/config/config.hpp` and use one of the functions

```c++
tao::config:value tao::config::parse_file( const std::string& file );
tao::config:value tao::config::parse_files( const std::vector< std::string >& files );
```

to obtain a `tao::config::value`, which is a `tao::json::basic_value< tao::config::traits >`.
Then the usual [JSON Value API](https://github.com/taocpp/json) can be used to inspect the obtained config.

The returned [JSON Values](https://github.com/taocpp/json) use the JSON libraries annotation feature to annotate the values with a JSON pointer and the filename and position they were parsed from.

Parsing multiple files with `parse_files()` does the same as having each config file include the next one in the list at the end (or having a single extra config file include all others).

Alternative versions of these functions take a template parameter for a `tao::json::basic_value<>` with different traits as result.

```c++
template< template< typename... > class Traits >
tao::json::basic_value< Traits > tao::config::basic_parse_file( const std::string& file );
template< template< typename... > class Traits >
tao::json::basic_value< Traits > tao::config::basic_parse_files( const std::vector< std::string >& files );
```

The pointer and position information is given to each result (sub-)value via the public base class' `set_pointer()` and `set_position()` methods, see `tao/config/annotation.hpp`.
These methods need not be implemented if the pointer and position information is not required.

For further details please Use The Source, or contact us if you have any questions.

Copyright (c) 2018 Dr. Colin Hirsch and Daniel Frey
