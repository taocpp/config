# Writing Config Files

---

The config file syntax is based on [JSON] and is completely backwards compatible:
**Every [JSON]** or [JAXN] **file** with a top-level [JSON] object **is a valid [taoCONFIG] file**.

The data model is also based on [JSON] and corresponds exactly to the [JAXN] data model as implemented by the [taoJSON] library.
We assume that the reader is somewhat familiar with [JSON].

 * [General Syntax](#general-syntax)
   - [Example](#example)
   - [Comments](#comments)
   - [Member Names](#member-names)
   - [Implicit Object](#implicit-object)
   - [Implicit Commas](#implicit-commas)
   - [Trailing Commas](#trailing-commas)
   - [Equality Sign](#equality-sign)
 * [Atomic Values](#atomic-values)
   - [Literal Names](#literal-names)
   - [Number Values](#number-values)
   - [String Values](#string-values)
   - [Binary Values](#binary-values)
 * [Additional Features)(#additional-features)
   - [Overwrite](#overwrite)
   - [Delete](#delete)
   - [Temporary](#temporary)
   - [References](#references)
   - [Extensions](#extensions)

Please note that, for now, this document shows basic use cases for all features, but not much beyond that.
It also does not go into the details and complexities of how all the features (can) interact with each other.



# General Syntax

This first section is in tutorial form.


## Example

The config file syntax is based on [JSON] and is completely backwards compatible:
**Every [JSON]** or [JAXN] **file** with a top-level [JSON] object **is a valid [taoCONFIG] file**.

In particular curly braces and square brackets have the same role as in [JSON], namely defining objects (key-value mappings where the keys are strings) and arrays, respectively.

The following very simple example [JSON] file will be transformed by using the syntactic possibilities of [taoCONFIG] in the order they are introduced below.

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

Member names can use single quotation marks instead of the standard [JSON] double quotation marks.

Member names that are C-style identifiers (non-empty sequences of ASCII characters, digits and underscores that do not start with a digit) can be written without quotation marks.

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

Note that every single separating comma is individually optional.


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



# Atomic Values

Atomic values are just as in [JSON] and [JAXN].


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



# Additional Features

Features that extend the syntax and/or semantics beyond simple syntactic sugar for [JSON].


## Overwrite

The same object member name can be assigned to multiple times.
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


## Delete

Previously defined values can be deleted with the literal pseudo-value `delete`.

#### Example taoCONFIG Input File

```
#!/usr/local/bin/qs

ip = "127.0.0.2"
port = 27960
maps = [ "ztn" "dm13" "t9" ]
maps = delete  // Changed our minds, no maps.
```

#### Resulting JSON Config Data

```javascript
{
   "ip": "127.0.0.2",
   "port": 27960
}
```

TODO


## Dotted Names

In many (all?) places the name of an object member is used it is actually possible to use a name with multiple dot-separated components, similar but not identical to JSON Pointer.

Dotted names can contain four different kinds of components.

1.  Strings intended to index objects.
2.  Unsigned integers intended to index arrays.
4.  The character `*` that indicates all sub-values.

Unlike JSON Pointer, integers are never interpreted as the name of an object member.
Just as for single-component names, integers, and other strings that are not C-style identifiers can be used with (single or double) quotes.
For example the dotted name `foo."1.2.3".bar` consists of three components, the strings `"foo"`, `"1.2.3"` and `"bar"`.


## References

TODO (syntax, semantics, nesting, phasing, vs. temporary, vs. equals)



## Extensions

Extensions are special expressions that, like references, are written in parentheses and greatly extend the possibilities of the config file.

[Value extensions](Value-Extensions.md) produce a [JAXN] value and can be used wherever a value is expected.

For example the `split` value extension takes a string and splits it into it's space-separated components.

```
foo = (split "a 1 ?")  // foo = [ "a", "a", "?" ]
```

[Member extensions](Member-Extensions.md) use the same syntax as value extensions, however they take the place of JSON object members.

For example the `include` member extensions takes a string, interprets it as filename, and reads and parses its contents in place of the `include` extension.

```
(include "filename.cfg")
```

For a list and description of all available extensions please read the pages on [value extensions](Value-Extensions.md) and [member extensions](Member-Extensions.md).

Value extensions can be nested quite freely, however the inner-most arguments must be literals as extensions can *not* access config values or use references.

For example the `split`, `read` and `env` extensions can be combined and nested like this.

```
foo = (split (read (env "IP_LIST_FILE")))
```

On the other hand moving the inner-most argument into a separate definition and accessing it from inside the nested extensions does *not* work.

```
bar = "IP_LIST_FILE
foo = (split (read (env (bar))))  // Parse error!
```

Member extensions can similarly contain nested value extensions.

```
(include (read (env "INCLUDE_FILE_NAME_FILE")))
```



# Temporary

The literal pseudo-value `temporary` (and its counterpart, `permanent`) can occur anywhere one of the literal values `null`, `true` and `false` can occur, however it does not itself carry a value.
Instead it marks the sub-tree of the JSON result to which it is "added' as temporary, meaning that while it is available to references it will be removed from the final resulting JSON value.

#### Example taoCONFIG Input File

```
template = temporary +
{
    host = "127.0.0.1"
    port = 6000
    version = 42
}

foo = (template) +
{
    host = "127.0.0.2"
}

bar = (template) +
{
    port = 6001
}

template += temporary  // Can also be "added" later.

```

#### Resulting JAXN Config Data

```javascript
{
   bar: {
      host: "127.0.0.1",
      port: 6001,
      version: 42
   },
   foo: {
      host: "127.0.0.2",
      port: 6000,
      version: 42
   }
}
```

Similarly `permanent` allows removing the marking as temporary.
Arbitrary many `temporary` and `permanent` pseudo-values can be "added" to a value with the last one "winning".

#### Example taoCONFIG Input File

```
foo = temporary + permanent + 42 + permanent + temporary
bar = temporary + temporary + 42 + permanent + permanent
```

#### Resulting JAXN Config Data

```javascript
{
   bar: 42
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
