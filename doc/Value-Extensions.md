# Value Extensions

Value extensions produce a [JAXN] value and can be used wherever a value is expected.
Note that whitespace is significant within value extensions, i.e. whitespace MUST be used as shown and comments are forbidden.

 * [binary](#binary)
 * [cbor](#cbor)
 * [default](#default)
 * [env](#env)
 * [jaxn](#jaxn)
 * [json](#json)
 * [msgpack](#msgpack)
 * [parse](#parse)
 * [read](#read)
 * [shell](#shell)
 * [split](#split)
 * [string](#string)
 * [ubjson](#ubjson)



## binary

The `binary` value extension explicitly transforms a string value into a binary value.
Only the type is changed, the represented sequence of bytes is not changed.

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



## cbor

The `cbor` value extension parses binary data as [CBOR] and returns the resulting value.

#### Example taoCONFIG Input File

```
foo = (cbor $82f5f4)
```

#### Resulting JAXN Config Data

```javascript
{
   foo: [
      true,
      false
   ]
}
```

Note that, in line with the JSON data model, only UTF-8 strings are supported as keys in CBOR mappings.

Note that `cbor` is frequently combined with `read` as in `foo = (cbor (read "filename.cbor"))`.



## default



## env

The `env` value extensions obtain the value of an environment variable as string.
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

The `jaxn` value extension parses string (or binary) data as [JAXN] and returns the resulting value.
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

Note that `jaxn` is frequently combined with `string` and `read` as in `foo = (jaxn (read "filename.jaxn"))`.



## json

The `json` value extension parses string (or binary) data as [JSON] and returns the resulting value.
In the case of binary data the input is automatically converted to a string, including a check for valid UTF-8.

#### Example taoCONFIG Input File

```
foo = (json '["a","b"]')
```

#### Resulting JAXN Config Data

```javascript
{
   foo: [
      "a",
      "b"
   ]
}
```

Note that `json` is frequently combined with `read` as in `foo = (json (read "filename.json"))`.



## msgpack

The `msgpack` value extension parses binary data as [MsgPack] and returns the resulting value.

#### Example taoCONFIG Input File

```
foo = (msgpack $82a161c3a162c2)
```

#### Resulting JAXN Config Data

```javascript
{
   foo: {
      a: true,
      b: false
   }
}
```

Note that `msgpack` is frequently combined with `read` as in `foo = (msgpack (read "filename.msgpack"))`.



## parse

The `parse` value extension parses the given string as a single config value just "as if" the config file contained the string instead of the invocation of `parse`.

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

Note that the value described in the string is *not* allowed to use addition/concatenation, however references and/or other value extensions *are* allowed.



## read

The `read` file extension reads the contents of a file and returns them as binary data.

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

Note that the conversion from `binary` to `string` is automatic when the binary data is passed to an extension that expects a string.



## shell

The `shell` value extension execute the given string as shell script and returns its output.

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
Currently it is only supported on Unix-style operating system like Linux and macOS.



## split

The `split` value extensions splits a string into its space-separated components and returns an array of them where "space" is a non-empty sequence of `' ', '\n', '\r', '\t', '\v' and/or '\f'` characters.

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

The `string` value extension transforms a binary value into a string value.
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



## ubjson

The `ubjson` value extension parses binary data as [UBJSON] and returns the resulting value.

#### Example taoCONFIG Input File

```
foo = (ubjson $4344)
```

#### Resulting JAXN Config Data

```javascript
{
   foo: "D"
}
```

Note that `ubjson` is frequently combined with `read` as in `foo = (ubjson (read "filename.ubjson"))`.



Copyright (c) 2018-2023 Dr. Colin Hirsch and Daniel Frey

[CBOR]: http://cbor.io
[JAXN]: https://github.com/stand-art/jaxn
[JSON]: https://tools.ietf.org/html/rfc8259
[MsgPack]: http://msgpack.org
[taoCONFIG]: https://github.com/taocpp/config
[taoJSON]: https://github.com/taocpp/json
[UBJSON]: http://ubjson.org
