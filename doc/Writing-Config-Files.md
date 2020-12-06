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
   - [Deleting](#deleting)
 * [Atomic Values](#atomic-values)
   - [Literal Names](#literal-names)
   - [Number Values](#number-values)
   - [String Values](#string-values)
   - [Binary Values](#binary-values)
 * [References](#references)
 * [Extensions](#extensions)



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


## Deleting

Previously defined values can be deleted with the special literal value `delete`.

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

TODO


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



# Extensions

[value extensions](Value-Extenions.md) and [member extensions](Member-Extensions.md)


Copyright (c) 2018-2020 Dr. Colin Hirsch and Daniel Frey

[CBOR]: http://cbor.io
[JAXN]: https://github.com/stand-art/jaxn
[JSON]: https://tools.ietf.org/html/rfc8259
[MsgPack]: http://msgpack.org
[taoCONFIG]: https://github.com/taocpp/config
[taoJSON]: https://github.com/taocpp/json
[UBJSON]: http://ubjson.org
