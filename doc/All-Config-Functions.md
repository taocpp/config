# All Config Functions

 * [binary](#binary)
 * [default](#default)
 * [env](#env)
 * [jaxn](#jaxn)
 * [parse](#parse)
 * [print](#print)
 * [read](#read)
 * [shell](#shell)
 * [split](#split)
 * [string](#string)

This page is the reference documentation for all included config functions.

The [general information on functions and how to use them can be found here.](Writing-Config-Files.md#functions)


## binary

The `binary` function explicitly transforms a string value into a binary value.
Only the type is changed, the sequence of bytes stays the same.

#### Example taoCONFIG Input File

```
foo = (binary "Hello, world!")
```

#### Resulting JAXN Config Data

```javascript
{
   foo: $48656C6C6F2C20776F726C6421
}
```


## default

The `default` function takes one or more arguments and returns the first one that is not a JSON `null`.
It is an error if all arguments are `null`.

#### Example taoCONFIG Input File

```
foo = (default 1 2)
bar = (default null false true)
```

#### Resulting JAXN Config Data

```javascript
{
   bar: false,
   foo: 1
}
```


## env

The `env` functions obtain the value of an environment variable as string.
For plain `env` it is an error when the environment variable does not exist, the `env?` alternative form returns a default value.

#### Example taoCONFIG Input File

```
foo = (env "USER")
bar = (env? "GRMBLFX" "default")
```

#### Resulting JAXN Config Data

```javascript
{
   bar: "default",
   foo: "colin"
}
```


## jaxn

The `jaxn` function parses string (or binary) data as [JAXN] and returns the resulting value.
In the case of binary data the input is automatically converted to a string, including a check for valid UTF-8.

#### Example taoCONFIG Input File

```
foo = (jaxn '[Infinity, $ff]')
```

#### Resulting JAXN Config Data

```javascript
{
   foo: [
      Infinity,
      $FF
   ]
}
```

Note that `jaxn` is frequently combined with `read` as in `foo = (jaxn (read "filename.jaxn"))`.


## parse

The `parse` function parses the given string as a single config value just "as if" the config file contained the string instead of the invocation of `parse`.

#### Example taoCONFIG Input File

```
foo = (parse "null")
```

#### Resulting JAXN Config Data

```javascript
{
   foo: null
}
```

This can be useful when combined with [`env`](#env) for environment variables that contain numeric values as in `foo = (parse (env "MYVAR"))`.

Note that the value described in the string is *not* allowed to use addition/concatenation, however references and functions *are* allowed.
Further, the `parse` function can **only** be used on "top-level", not inside of arguments to other functions.

For example `foo = (parse (default ...))` is allowed, but `foo = (default (parse ...) ...)` is not.


## print

The `print` function takes a part of the config and turns it back into a JAXN string (which is the same as a JSON string as long as the JAXN extensions to the JSON data model, binary data and non-finite floating point numbers, are **not** used).

#### Example taoCONFIG Input File

```
(temporary foo)

foo
{
    b1 = true
    b2 = "Hello, Test!"
    b3 = [ $00, $deadbeef ]
    b4 = 42
}

str = (print (foo))
```

#### Resulting JAXN Config Data

```javascript
{
   str: "{b1:true,b2:\"Hello, Test!\",b3:[$00,$DEADBEEF],b4:42}"
}
```


## read

The `read` function returns the contents of a file as binary data.

#### Example taoCONFIG Input File

```
foo = (read "tests/doc_value_read.config")
bar = (string (read "tests/doc_value_read.config"))
```

#### Resulting JAXN Config Data

```javascript
{
   bar: "foo = (read \"tests/doc_value_read.config\")\nbar = (string (read \"tests/doc_value_read.config\"))\n",
   foo: $666F6F203D202872656164202274657374732F646F635F76616C75655F726561642E636F6E66696722290A626172203D2028737472696E67202872656164202274657374732F646F635F76616C75655F726561642E636F6E6669672229290A
}
```

Note that `read` can be combined with `string` to validate the data as UTF-8 and transform it into a string.

Note that the conversion from `binary` to `string` is automatic when the binary data is passed to a function that expects a string.
Like [`string`](#string), the automatic conversion checks whether the binary data is a valid UTF-8 sequence and throws an exception if that is not the case.


## shell

The `shell` function executes the given string as shell script and returns its output.

#### Example taoCONFIG Input File

```
foo = (shell "uname -s")
```

#### Resulting JAXN Config Data

```javascript
{
   foo: "Darwin\n"
}
```

Note that availability and behaviour of the `shell` value extension are inherently system dependent.
Currently it is only supported on Unix-style operating systems that are sufficiently POSIX compliant, most prominently Linux and macOS.


## split

The `split` function splits a string into its space-separated components and returns an array of them.
Here "space" is a non-empty sequence of `' ', '\n', '\r', '\t', '\v' and/or '\f'` characters.

#### Example taoCONFIG Input File

```
foo = (split "a b c ")
```

#### Resulting JAXN Config Data

```javascript
{
   foo: [
      "a",
      "b",
      "c"
   ]
}
```


## string

The `string` function transforms a binary value into a string value and leaves string values unchanged.
It validates that the binary data is valid UTF-8 and produces an error if that is not the case.

#### Example taoCONFIG Input File

```
foo = (string $48656C6C6F2C20776F726C6421)
```

#### Resulting JAXN Config Data

```javascript
{
   foo: "Hello, world!"
}
```

Note that the conversion from `binary` to `string` is automatic when the binary data is passed to an extension that expects a string.
The automatic conversion, too, checks whether the binary data is a valid UTF-8 sequence and throws an exception if that is not the case.



Copyright (c) 2018-2024 Dr. Colin Hirsch and Daniel Frey

[CBOR]: http://cbor.io
[JAXN]: https://github.com/stand-art/jaxn
[JSON]: https://tools.ietf.org/html/rfc8259
[MsgPack]: http://msgpack.org
[taoCONFIG]: https://github.com/taocpp/config
[taoJSON]: https://github.com/taocpp/json
[UBJSON]: http://ubjson.org
