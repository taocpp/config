# Writing Config Files

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
 * [Additional Features](#additional-features)
   - [Additions](#additions)
   - [Overwrite](#overwrite)
   - [Delete](#delete)
   - [Asterisks](#asterisk)
   - [References](#references)
   - [Extensions](#extensions)
   - [Dotted Names](#dotted-names)

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

Numbers are like in [JAXN], i.e. [JSON] numbers with [extensions](https://github.com/stand-art/jaxn/blob/main/Specification.md#numbers).


## String Values

Strings are like in [JAXN], i.e. [JSON] strings with [extensions](https://github.com/stand-art/jaxn/blob/main/Specification.md#strings).


## Binary Values

[Binary data](https://github.com/stand-art/jaxn/blob/main/Specification.md#binary-data) is also like in [JAXN].



# Additional Features

Features that extend the syntax and/or semantics beyond simple syntactic sugar for [JSON].


## Additions

Values of certain types can be added together explicitly or, in some cases, implicitly.

#### Numbers

For numbers the addition must be explicit and performs the usual addition, however:

Given that there are three data types for numeric values, signed integer, unsigned integer, and floating point, each with 64bits, the following rules are applied.

 * Integer values of the same signedness yield an integer with that signedness as result.
 * Integer values of mixed signedness are converted to signed integers before adding and yield a signed integer as result.
 * Floating-point values can only be added to other floating-point values and yield a floating-point value as result.

The order in which the additions are performed is undefined, e.g. `foo = 1 + 2 + 3` can be evaluated as either `(1 + 2) + 3` or `1 + (2 + 3)`.

```
foo = 42 + 10
foo += 3
// foo is 55
```

#### Strings

For strings the addition must be explicit and performs concatenation.

```
foo = "a" + "b"
foo += "c"
// foo is "abc"
```

Like in [JAXN] the same concatenations can be performed for binary data, and strings and binary data can not be mixed.

#### Arrays

For arrays the addition can be explicit or implicit and performs concatenation.

The implicit form is obtained by omitting a `+=` after a name.

```
foo = [ 1 2 ] + [ 3 ]
foo += [ 4 5 ]
foo [ 6 7 ]
// foo is [ 1 2 3 4 5 6 7 ]
```

#### Objects

For objects the addition can be explicit or implicit and performs an object "merge" operation.

For keys that exist only in one of the two objects being merged the result of the merge will contain that object member unchanged.

For keys that exist in both of the objects being merged the value of the member in the result will depend on whether the second objects used a `=` or a `+=` to define the member.
In the first case the member from the second object overwrites the member from the first, in the second case the corresponding values are in turn added.

#### Example taoCONFIG Input File

```
foo
{
    a = 1
    b = 2
    c = 3
}
foo
{
    a = 10
    b += 20
    d = 40
}
```

#### Resulting JSON Config Data

```javascript
{
   foo: {
      a: 10,
      b: 22,
      c: 3,
      d: 40
   }
}
```

## Overwrite

A named object member can be assigned to multiple times in which case the last assignment "wins".

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

Deleted values can subsequently be assigned new values or added-to in which case they behave "as if" anything that was assigned or added prior to the `delete` never happened.

```
foo = 42
foo = delete
foo += 44
```

Unlike the other pseudo-values `delete` is not allowed to be in a syntactic addition.


## Asterisks

The asterisk can be used as special object member name to designate all array or object members.
It is frequently used in conjunction with [dotted names](#dotted-names) to form a pattern for an assignment or addition.

#### Example taoCONFIG Input File

```
servers
{
    primary
    {
	port = 443
    }
    secondary
    {
        port = 8888
    }
}
servers.*.port = 7000  // Testing
```

#### Resulting JSON Config Data

```javascript
{
   servers: {
      primary: {
         port: 7000
      },
      secondary: {
         port: 7000
      }
   }
}
```

Assignments using asterisks can be combined with the literal pseudo-value `delete`.


## References

References are (nested) dotted names written in parentheses that refer to other parts of the config and copy the referenced value.

```
foo = 42
bar = (foo)
// bar is 42
```

To uniformly support forwards and backwards references, they and [additions](#additions) are resolved *after* the config has been parsed, i.e. after all assignments and [extensions](#extensions).

References are resolved relative to the array or object scope in which they syntactically occur, similar to how name lookups from within nested namespaces are handled in the C++ programming language.

#### Example taoCONFIG Input File

```
r = (a.b.x)

a
{
    b
    {
        c
        {
            i = (x)
            j = (b.x)
            k = (a.x)

            x = 4
	}
        x = 3
    }
    x = 2
}
x = 1
```

#### Resulting JSON Config Data

```javascript
{
   a: {
      b: {
         c: {
            i: 4,
            j: 3,
            k: 2,
            x: 4
         },
         x: 3
      },
      x: 2
   },
   r: 3,
   x: 1
}
```

It is an error if there is no order in which all additions and references can be resolved due to some kind of cyclic references.
It is also an error when the first component of a reference matches an inner scope in which the other parts can not be resolved *even if* they could have been resolved in an enclosing scope.

#### Example Broken Input Files

Cyclic reference is an error:

```
a = (b)
b = (a)
```

While resolving `(a.b)` within `a.a` the first time the first `a` of `(a.b)` matches while moving from inner to outer scopes is the inner `a` in which no `b` exists which is an error:

```
a
{
    a
    {
        c = (a.b)
    }
    b = 1
}
```

References can be arbitrarily **nested** meaning that the parts ofa  reference can themselves be references.
For this to work the inner references must refer to values that can occur as part of a reference, namely strings and integers.

#### Example taoCONFIG Input File

```
foo = [ 10 11 12 13 14 ]
bar = 3
baz = (foo.(bar))
```

#### Resulting JSON Config Data

```javascript
{
   bar: 3,
   baz: 13,
   foo: [
      10,
      11,
      12,
      13,
      14
   ]
}
```

When referencing values marked as [temporary](Member-Extensions.md#temporary) the copied value will not be marked as temporary, however any sub-values will retain their temporary status.
Similarly schema definitions will be retained only for sub-values of the referenced value (TODO: Is this really what we want?)

#### Example taoCONFIG Input File

```
(temporary foo)
(temporary foo.baz)

foo
{
    bar
    {
        a = 1
    }
    baz
    {
        b = 2
    }
}

ttt = (foo)
```

#### Resulting JSON Config Data

```javascript
{
   ttt: {
      bar: {
         a: 1
      }
   }
}
```


## Extensions

Extensions are expressions that, like references, are written in parentheses and give access to same *extended* features.

[Value extensions](Value-Extensions.md) produce a [JAXN] value and can be used wherever a value is expected.

For example the `split` value extension splits a string into its space-separated components.

```
foo = (split "a 1 ?")  // foo = [ "a", "a", "?" ]
```

[Member extensions](Member-Extensions.md) use the same syntax as value extensions, however they take the place object members.

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


## Dotted Names

Wherever the name of an object member is expected it is also possible to use a name with multiple dot-separated components.

Dotted names can contain kinds of components.

1.  Strings, intended to index objects.
2.  Unsigned integers, intended to index arrays.
4.  The [asterisk](#asterisks), intended to index objects or arrays.

#### Example taoCONFIG Input File

```
foo
{
    bar.baz += 1
}
foo.bar
{
    baz += 1
}
foo.bar.baz += 1
```

#### Resulting JSON Config Data

```javascript
{
   foo: {
      bar: {
         baz: 3
      }
   }
}
```

Just as for single-component names, integers, and other strings that are not C-style identifiers can be used with (single or double) quotes.
For example the dotted name `foo."1.2.3".bar` consists of three components, the strings `"foo"`, `"1.2.3"` and `"bar"`.

Dotted names can be thought of as strongly-typed JSON Pointers: there is no ambiguity about whether an integer is intended to index an array or an object.


Copyright (c) 2018-2022 Dr. Colin Hirsch and Daniel Frey

[CBOR]: http://cbor.io
[JAXN]: https://github.com/stand-art/jaxn
[JSON]: https://tools.ietf.org/html/rfc8259
[MsgPack]: http://msgpack.org
[taoCONFIG]: https://github.com/taocpp/config
[taoJSON]: https://github.com/taocpp/json
[UBJSON]: http://ubjson.org
