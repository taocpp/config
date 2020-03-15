# Writing Config Files

*Warning: The documentation is still work-in-progress and very incomplete.*

The config file syntax is based on [JSON] and is completely backwards compatible:

**Every [JSON]** or [JAXN] **file** with a top-level Object **is a valid [taoCONFIG] file**.

The data model is also based on [JSON] and corresponds exactly to the [JAXN] data model as implemented by the [taoJSON] library.

We assume that the reader is already somewhat familiar with [JSON].



## General Syntax

 * [Example](#example)
 * [Comments](#comments)
 * [Object Keys](#object-keys)
 * [Implicit Object](#implicit-object)
 * [Implicit Commas](#implicit-commas)
 * [Trailing Commas](#trailing-commas)
 * [Equality Sign](#equality-sign)
 * [Overwriting](#overwriting)

## Atomic Values

 * [Literal Names](#literal-names)
 * [Number Values](#number-values)
 * [String Values](#string-values)
 * [Binary Values](#binary-values)

## Advanced Syntax

* [Complex Keys](#complex-keys)

## Advanced Features

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
   - [stderr](#stderr)
   - [temporary](#temporary)



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

The curly braces for the top-level Object are optional and can be omitted.

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

The commas separating Array elements and Object members are also optional and can be omitted.

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

A single trailing comma is permitted after the last Array element or last Object member, including after the last member of the implicit top-level Object.

#### Example taoCONFIG Input File

```
#!/usr/local/bin/qs

ip: "127.0.0.2",
port: 27960
maps: [ "ztn" "dm13", "t9", ]  // Add dm6 or t4?
,
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

The equality sign `=` can be used instead of the standard [JSON] colon `:` to separate the key and value of an Object member.

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

The same Object key can be assigned to multiple times in which case the last assignments "wins".

#### Example taoCONFIG Input File

```
#!/usr/local/bin/qs

port = 26000
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

Note that overwriting earlier values can be done on top-level or in any arbitrarily deeply nested part of the config, and it does not need to preserve the type of the value.



## Literal Names

The three literal names `null`, `true` and `false` are copied over from [JSON].

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



## Number Values



## String Values



## Binary Values



## Complex Keys



## References



## Value Extensions

Value extensions produce a single value and can be used wherever a single value was expected.

### binary

### cbor

### copy

### debug

### env

### jaxn

### json

### msgpack

### parse

### read

### shell

Note that the `shell` value extension requires Unix or Linux or macOS.

### split

### string

### ubjson



## Member Extensions

Member extensions use the same syntax as value extensions, however they take the place of JSON Object members.

### delete

### include

### schema

### stderr

This member extension is a debugging tool.
It prints a JSON representation of the library's internal intermediate data structure to the standard error channel.
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

### temporary

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
