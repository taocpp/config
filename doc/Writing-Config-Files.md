# Writing Config Files

*Warning: The documentation is still work-in-progress and very incomplete.*

---

The config file syntax is based on [JSON] and is completely backwards compatible:
**Every [JSON]** or [JAXN] **file** with a top-level [JSON] object **is a valid [taoCONFIG] file**.

The data model is also based on [JSON] and corresponds exactly to the [JAXN] data model as implemented by the [taoJSON] library.
We assume that the reader is at least somewhat familiar with [JSON].

It should be kept in mind that evaluating a config file to a [JSON] object is performed in [two phases](Two-Phase-Evaluation.md).
1. The first phase, which takes care of everything besides [references](#references) and addition/concatenation, is imperative in nature in that everything happens in the order it occurs in the config file(s).
2. The second phase, which takes care of resolving [references](#references) and performing additions and concatenations, is more declarative in nature in that order is not important as long as there are no cycles.

Please note that, for now, this document shows basic use cases for all features, but not much beyond that.
It also does not go into the details and complexities of how all the features (can) interact with each other.


 * [General Syntax](#general-syntax)
   - [Example](#example)
   - [Comments](#comments)
   - [Member Names](#member-names)
   - [Implicit Object](#implicit-object)
   - [Implicit Commas](#implicit-commas)
   - [Trailing Commas](#trailing-commas)
   - [Equality Sign](#equality-sign)
   - [Dotted Names](#dotted-names)
   - [Overwriting](#overwriting)
 * [Atomic Values](#atomic-values)
   - [Literal Names](#literal-names)
   - [Number Values](#number-values)
   - [String Values](#string-values)
   - [Binary Values](#binary-values)
 * [References](#references)
 * [Value Extensions](#value-extensions)
   - [binary](#binary)
   - [cbor](#cbor)
   - [copy](#copy)
   - [debug](#debug)
   - [env](#env)
   - [jaxn](#jaxn)
   - [json](#json)
   - [msgpack](#msgpack)
   - [parse](#parse)
   - [read](#read)
   - [shell](#shell)
   - [split](#split)
   - [string](#string)
   - [ubjson](#ubjson)
 * [Member Extensions](#member-extensions)
   - [delete](#delete)
   - [include](#include)
   - [schema](#schema)
   - [setenv](#setenv)
   - [stderr](#stderr)
   - [temporary](#temporary)



# General Syntax

This section is in tutorial form.
It takes a simple [JSON] compliant config file and introduces the general syntactic extensions featured by [taoCONFIG] one by one, and also shows how the example can be subsequently transformed to a more config-like style.


## Example

This very simple example [JSON] file will be transformed by using the syntactic possibilities of [taoCONFIG] as successively introduced in the following sections.

#### Example JSON Config File

```javascript
{
   "ip": "127.0.0.2",
   "port": 27960,
   "maps": [ "ztn", "dm13", "t9" ]
}
```


## Comments

All comments that are allowed in [JAXN] can be used in config files, namely

* "shell-style" until-end-of-line with `#`,
* "C++-style" until-end-of-line with `//`,
* and "C-style" non-nesting block comments with `/*` and `*/`.

#### Example taoCONFIG Input File

```
#!/usr/local/bin/qs
{
   "ip": "127.0.0.2",
   "port": 27960,
   "maps": [ "ztn", "dm13", "t9" ]  // Add dm6 or t4?
}
```

#### Resulting JSON Config Data

```javascript
{
   "ip": "127.0.0.2",
   "maps": [
      "ztn",
      "dm13",
      "t9"
   ],
   "port": 27960
}
```


## Member Names

member names can use single quotation marks instead of the standard [JSON] double quotation marks.

member names that are C-style identifiers (non-empty sequences of ASCII characters, digits and underscores that do not start with a digit) can be written without quotation marks.

#### Example taoCONFIG Input File

```
#!/usr/local/bin/qs
{
   ip: "127.0.0.2",
   "port": 27960,
   'maps': [ "ztn", "dm13", "t9" ]  // Add dm6 or t4?
}
```

#### Resulting JSON Config Data

```javascript
{
   "ip": "127.0.0.2",
   "maps": [
      "ztn",
      "dm13",
      "t9"
   ],
   "port": 27960
}
```


## Implicit Object

The curly braces for the top-level object are optional and can be omitted.

#### Example taoCONFIG Input File

```
#!/usr/local/bin/qs

ip: "127.0.0.2",
port: 27960,
maps: [ "ztn", "dm13", "t9" ]  // Add dm6 or t4?
```

#### Resulting JSON Config Data

```javascript
{
   "ip": "127.0.0.2",
   "maps": [
      "ztn",
      "dm13",
      "t9"
   ],
   "port": 27960
}
```


## Implicit Commas

The commas separating array elements and object members are also optional and can be omitted.

#### Example taoCONFIG Input File

```
#!/usr/local/bin/qs

ip: "127.0.0.2"
port: 27960
maps: [ "ztn" "dm13" "t9" ]  // Add dm6 or t4?
```

#### Resulting JSON Config Data

```javascript
{
   "ip": "127.0.0.2",
   "maps": [
      "ztn",
      "dm13",
      "t9"
   ],
   "port": 27960
}
```

Note that every single separating comma is individually optional and can be included or omitted.


## Trailing Commas

A single trailing comma is permitted after the last array element or last object member, including after the last member of the implicit top-level object.

#### Example taoCONFIG Input File

```
#!/usr/local/bin/qs

ip: "127.0.0.2",
port: 27960,
maps: [ "ztn",
        "dm13",
        "t9", ],  // Add dm6 or t4?
```

#### Resulting JSON Config Data

```javascript
{
   "ip": "127.0.0.2",
   "maps": [
      "ztn",
      "dm13",
      "t9"
   ],
   "port": 27960
}
```


## Equality Sign

The equality sign `=` can be used instead of the standard [JSON] colon `:` to separate the name and value of an object member.

#### Example taoCONFIG Input File

```
#!/usr/local/bin/qs

ip = "127.0.0.2"
port = 27960
maps = [ "ztn" "dm13" "t9" ]  // Add dm6 or t4?
```

#### Resulting JSON Config Data

```javascript
{
   "ip": "127.0.0.2",
   "maps": [
      "ztn",
      "dm13",
      "t9"
   ],
   "port": 27960
}
```


## Overwriting

The same member name can be assigned to multiple times.
The last assignment "wins".

#### Example taoCONFIG Input File

```
#!/usr/local/bin/qs

port = "TODO!"
ip = "127.0.0.2"
port = 27960
maps = [ "ztn" "dm13" "t9" ]  // Add dm6 or t4?
```

#### Resulting JSON Config Data

```javascript
{
   "ip": "127.0.0.2",
   "maps": [
      "ztn",
      "dm13",
      "t9"
   ],
   "port": 27960
}
```

Note that overwriting earlier values can be done on top-level or in any arbitrarily deeply nested part of the config.


## Dotted Names

In many (all?) places the name of an object member is used it is actually possible to use a name with multiple dot-separated components, similar but not identical to JSON Pointer.

Dotted names can contain four different kinds of components.

1.  Strings intended to index objects.
2.  Unsigned integers intended to index arrays.
3.  The character `-` that denotes the last array element.
4.  The character `*` that indicates all sub-values.

Unlike JSON Pointer, integers are never interpreted as the name of an object member.
Just as for single-component names, Integers, and other strings that are not C-style identifiers can be used with (single or double) quotes.
For example the dotted name `foo."1.2.3".bar` consists of three components, the strings `"foo"`, `"1.2.3"` and `"bar"`.
Further, since dotted names are always resolved within an object, rather than an array, they must begin with some kind of string.


# Atomic Values


## Literal Names

The three literal names `null`, `true` and `false` are carred over from [JSON].

#### Example taoCONFIG Input File

```
a = null
b = true
c = false
```

#### Resulting JSON Config Data

```javascript
{
   "a": null,
   "b": true,
   "c": false
}
```

Note that `null`, `true` and `false` as member names are shortcuts for `"null"`, `"true"` and `"false"`, respectively.


## Number Values

Numbers are like in [JAXN], i.e. [JSON] numbers with [extensions](https://github.com/stand-art/jaxn/blob/master/Specification.md#numbers).


## String Values

Strings are like in [JAXN], i.e. [JSON] strings with [extensions](https://github.com/stand-art/jaxn/blob/master/Specification.md#strings).


## Binary Values

[Binary data](https://github.com/stand-art/jaxn/blob/master/Specification.md#binary-data) is also like in [JAXN].



# Resolution

Object member names always occur in the syntactical scope of an object, and the names are understood to refer to 



# References



# Value Extensions

Value extensions produce a [JAXN] value and can be used wherever a value is expected.
Note that whitespace is significant within value extensions, i.e. whitespace must be used as shown and comments are forbidden.


## binary

The `binary` value extension transforms a string value into a binary value.

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

Note that `cbor` is frequently combined with `read` as in `foo = (cbor (read "filename.cbor"))`.


## copy

The `copy` value extension copies a value

#### Example taoCONFIG Input File

```

```

#### Resulting JAXN Config Data

```javascript
```


## debug

This value extension is a debugging tool and is similar to the `stderr` member extension.

It produces a single string value with the [JSON] representation of the library's internal intermediate data structure for the referenced part of the config data in its current state.

#### Example taoCONFIG Input File

```
foo = 42
bar = (debug foo)
```

#### Resulting JAXN Config Data

```javascript
{
   bar: "{position:\"tests/doc_value_debug.config:1:6\",concat_list:[{clear:true,atom:42}]}",
   foo: 42
}
```


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

The `jaxn` value extension parses string data as [JAXN] and returns the resulting value.

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

Note that `jaxn` is frequently combined with `string` and `read` as in `foo = (jaxn (string (read "filename.jaxn")))`.


## json

The `json` value extension parses string data as [JSON] and returns the resulting value.

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

Note that `json` is frequently combined with `string` and `read` as in `foo = (json (string (read "filename.json")))`.


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

Note that the value described in the string is *not* allowed to use addition/concatenation, however references and/or other value extensions are allowed.


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

Note that `read` must be combined with `string` in order to validate the data as UTF-8 and transform it into a string.


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

Note that availability and behaviour of the  `shell` value extension are inherently system dependent.
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



# Member Extensions

Member extensions use the same syntax as value extensions, however they take the place of JSON object members.
Note that whitespace is significant within member extensions, i.e. whitespace must be used as shown and comments are forbidden.


## delete

The `delete` member extensions delete a value (and all of its sub-values) from the config.
For plain `delete` it is an error if the value does not exist, the `delete?` alternative form is idempotent and never reports an error.
Values that were deleted can be assigned to again later.

#### Example taoCONFIG Input File

```
foo = "LG"
bar = "RL"

(delete foo)
(delete? baz)
```

#### Resulting JAXN Config Data

```javascript
{
   bar: "RL"
}
```


## include


## schema

The `schema` member extension tells the config library which [schema file](Writing-Schema-Files.cfg) the config must adhere to.
After reading the config file(s), the schema file from the last `schema` directive, if any, is read, and the config is checked against it.

#### Example taoCONFIG Input File

```
ip = "127.0.0.1"
port = 42

(schema "tests/doc_member_schema.schema")
```

#### Example taoCONFIG Schema File

```
definitions
{
    port
    {
        type: "std.unsigned"
        minimum: 1
        maximum: 65535
    }
}

properties
{
    required
    {
        ip: "std.net.ip_v4_address"
        port: "port"
    }
}
```

#### Resulting Printed Debug Data

```javascript
{
   ip: "127.0.0.1",
   port: 42
}
```

For more on schemas [please consult the page on how to write schema files](Writing-Schema-Files.md).


## setenv

*The `setenv` member extension only exists to make the examples independent of their environment.*


## stderr

This member extension is a debugging tool and is similar to the `debug` value extension.

It prints a JSON representation of the library's internal intermediate data structure for the referenced part of the config data in its current state to the standard error channel.
The config value itself does not change through the presence or absence of "stderr members".

#### Example taoCONFIG Input File

```
foo = 42

(stderr foo)
```

#### Resulting Printed Debug Data

```javascript
{
   position: "tests/doc_member_stderr.config:1:6",
   concat_list: [
      {
         clear: true,
         atom: 42
      }
   ]
}
```

#### Resulting JAXN Config Data

```javascript
{
   foo: 42
}
```


## temporary

A part of the config can be marked as _temporary_ meaning that it will be removed from the final result after reading is complete.
Marking a part of the config as _temporary_ can be done before or after the marked part of the config was (first) defined.
Temporary parts of the config can still be referenced and copied normally.

#### Example taoCONFIG Input File

```
template
{
   foo = 42
   bar = true
}

(temporary template)

first = (template) +
{
   foo = 43
}

second = (template) +
{
   bar = false
}
```

#### Resulting JAXN Config Data

```javascript
{
   first: {
      bar: true,
      foo: 43
   },
   second: {
      bar: false,
      foo: 42
   }
}

```



Copyright (c) 2018-2020 Dr. Colin Hirsch and Daniel Frey

[CBOR]: http://cbor.io
[JAXN]: https://github.com/stand-art/jaxn
[JSON]: https://tools.ietf.org/html/rfc8259
[MsgPack]: http://msgpack.org
[taoCONFIG]: https://github.com/taocpp/config
[taoJSON]: https://github.com/taocpp/json
[UBJSON]: http://ubjson.org
