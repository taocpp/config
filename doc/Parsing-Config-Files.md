# Parsing Config Files

*  [Parsing](#parsing)
*  [Inspecting](#inspecting)
*  [Annotations](#annotations)
*  [Custom Traits](#custom-traits)
*  [Builtin Schema](#builtin-schema)

This library requires decent C++17 support, currently tested on GCC 8 and 9 and "recent" Clang.

The following assumes that the [PEGTL], [taoJSON] and [taoCONFIG] are all available to your compiler, i.e. the include paths are set up correctly.
Then to access the facilities of this library please simply include `<tao/config.hpp>` in your source(s).

## Parsing

The following functions read and parse one or more config files.
The return value is a single [taoJSON] Object in the form of a `tao::config::value` aka. `tao::json::basic_value< tao::config::traits >`.

```c++
tao::config::value tao::config::from_file( const std::filesystem::path& );
tao::config::value tao::config::from_files( const std::vector< std::filesystem::path >& files );
```

When more than one file is passed to `from_files()` it behaves (mostly) like parsing a single file with the concatenated contents (the only difference being that while individual files may optionally contain top-level curly braces for the implicit top-level JSON Object these would produce an error in the concatenated file).

The config can also be parsed from a `std::string` instead of a file.
The second parameter, `source`, should be a string that describes the source of the data.
It is used in error messages to indicate not just the line and column numbers.
See the [PEGTL] documentation for more details.

```c++
tao::config::value tao::config::from_string( const std::string& data, const std::string& source );
```

## Inspecting

Since the parsed config is presented as single [taoJSON] value object, all the facilities from the [taoJSON] library can be used to inspect and operate on the obtained config.

## Annotations

By default, i.e. when using the included `tao::config::traits` for `tao::json::basic_value`, the [taoJSON] library's annotation feature is used to store the "config Key", as well as the filename (or, more generally, the source) and line and column numbers where they occurred in the parsed input.

The config Key is conceptually a kind of strongly typed JSON Pointer.
That is, unlike in a JSON Pointer, the string "2" and the integer 2 are distinguished.
Strings can only be used to index objects, and integers can only be used to index arrays.

The annotation feature sets up `tao::config::annotation` as public base class of every `tao::config::value` object, whereby the public data members of `tao::config::annotation` are available as public data members of every `tao::config::value`, the returned one and all of its sub-values.

These public data members are:

```c++
   tao::config::key key;
   tao::json::position position;  // tao/json/contrib/position.hpp
```

## Custom Traits

The parsing functions are also available in a "basic" version that takes the traits for the returned [taoJSON] value as template parameter.

```c++
template< template< typename... > class Traits >
tao::json:basic_value< Traits > tao::config::basic_from_file( const std::filesystem::path& );

template< template< typename... > class Traits >
tao::json:basic_value< Traits > tao::config::basic_from_files( const std::vector< std::filesystem::path >& );

template< template< typename... > class Traits >
tao::json::basic_value< Traits > tao::config::basic_from_string( const std::string& data, const std::string& source );
```

Beyond the possibilities given to the traits by the [taoJSON] library this also gives control over the annotation.

The type `tao::config::annotation` implements the following non-static member functions that are used by the library to set the corresponding annotations.

```c++
   void set_key( tao::config::key&& );
   void set_position( const tao::pegtl::position& );
```

However these functions are only called when they are available, so a custom traits class template can choose to omit the annotations by either not setting up a public base class for the [taoJSON] values or by not implementing either or both of these functions in the public base.

## Builtin Schema

All of the config parsing functions actually have a further argument, a const-reference to an object of type `tao::config::schema::builtin`.
This argument is defaulted to a default-constructed object of that type.

The role of this `builtin` instance is to provide the built-in schema definitions that can then be extended by the schema files.
The built-in definitions can be programmatically extended by passing an appropriately prepared instance of `tao::config::schema::builtin` as final argument to the parsing functions.

Copyright (c) 2018-2022 Dr. Colin Hirsch and Daniel Frey

[PEGTL]: https://github.com/taocpp/PEGTL
[taoCONFIG]: https://github.com/taocpp/config
[taoJSON]: https://github.com/taocpp/json
