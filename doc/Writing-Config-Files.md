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

Please note that, for now, this document shows basic use cases for all features, but not much else.
It also does not go into the details and complexities of how all the features (can) interact with each other.



 * [General Syntax](#general-syntax)
   - [Example](#example)
   - [Comments](#comments)
   - [Object Keys](#object-keys)
   - [Implicit Object](#implicit-object)
   - [Implicit Commas](#implicit-commas)
   - [Trailing Commas](#trailing-commas)
   - [Equality Sign](#equality-sign)
   - [Overwriting](#overwriting)
 * [Atomic Values](#atomic-values)
   - [Literal Names](#literal-names)
   - [Number Values](#number-values)
   - [String Values](#string-values)
   - [Binary Values](#binary-values)
 * [Complex Keys](#complex-keys)
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


## Object Keys

Object keys can use single quotation marks instead of the standard [JSON] double quotation marks.

Object keys that are C-style identifiers (non-empty sequences of ASCII characters, digits and underscores that do not start with a digit) can be written without quotation marks.

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

The equality sign `=` can be used instead of the standard [JSON] colon `:` to separate the key and value of an object member.

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

The same object key can be assigned to multiple times.
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

Note that `null`, `true` and `false` as object keys are shortcuts for `"null"`, `"true"` and `"false"`, respectively.


## Number Values

Numbers are like in [JAXN], i.e. [JSON] numbers with [extensions](https://github.com/stand-art/jaxn/blob/master/Specification.md#numbers).


## String Values

Strings are like in [JAXN], i.e. [JSON] strings with [extensions](https://github.com/stand-art/jaxn/blob/master/Specification.md#strings).


## Binary Values

[Binary data](https://github.com/stand-art/jaxn/blob/master/Specification.md#binary-data) is also like in [JAXN].


## Complex Keys


## References



# Value Extensions

Value extensions produce a single value and can be used wherever a single value was expected.


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


## copy


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


## json


## msgpack


## parse


## read


## shell

Note that the `shell` value extension requires Unix or Linux or macOS.


## split


## string

The `string` value extension transforms a binary value into a string value.
It validates that the binary data is valid UTF-8 and produces an error if not.

#### Example taoCONFIG Input File

```
foo = $48656C6C6F2C20776F726C6421
```

#### Resulting JAXN Config Data

```javascript
{
   foo: "Hello, world!"
}
```


## ubjson



# Member Extensions

Member extensions use the same syntax as value extensions, however they take the place of JSON object members.


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


## setenv

*The `setenv` member extension only exists to make the examples self-contained and independent of the environment they run in.*


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

[JAXN]: https://github.com/stand-art/jaxn
[JSON]: https://tools.ietf.org/html/rfc8259
[taoCONFIG]: https://github.com/taocpp/config
[taoJSON]: https://github.com/taocpp/json
