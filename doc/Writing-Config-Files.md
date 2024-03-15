# Writing Config Files

The config file syntax is based on [JSON] and is completely backwards compatible:

> **Every [JSON]** (or [JAXN]) **file** with a top-level [JSON] object **is a valid [taoCONFIG] file**.

The data model is also based on [JSON] and corresponds exactly to the [JAXN] data model as implemented by the [taoJSON] library.
We assume that the reader is somewhat familiar with [JSON] and its terminology.

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
   - [Asterisks](#asterisks)
   - [References](#references)
   - [Functions](#functions)
   - [Dotted Names](#dotted-names)
   - [Temporaries](#temporaries)
   - [Include Files](#include-files)
 * [Advanced Considerations](#advanced-considerations)



# General Syntax

This first section is in tutorial form.
It starts with a simple config file example in JSON form and successively introduces the main syntactic features of taoCONFIG by changing the example to use these features.


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

[Binary data](https://github.com/stand-art/jaxn/blob/main/Specification.md#binary-data) is also exactly like in [JAXN].



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

Like in [JAXN] the same concatenations can be performed for binary data, and strings and binary data can **not** be mixed.

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

For keys that exist in both of the objects being merged the value of the member in the result will depend on whether the second object used a `=` or a `+=` to define the member.
In the case of `=` the member from the second object overwrites the member of the first, in the case of `+=` the corresponding values are recursively merged.

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

A named object member can be assigned-to multiple times with the final assignment "winning".

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

#### Example taoCONFIG Input File

```
foo = 42
foo = delete
foo += 44
```

#### Resulting JSON Config Data

```javascript
{
   foo: 44
}
```

Note that `delete` may not be an operand of an addition, i.e. `42 + delete` or similar are **not** allowed.


## Asterisks

The asterisk can be used as special name to designate all array or object members.
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

Assignments using asterisks **can** be combined with the literal pseudo-value `delete`.


## References

References are (nested) dotted names written in parentheses that refer to other parts of the config.
During processing of the config file they copy the referenced value.

```
foo = 42
bar = (foo)
// bar is 42
```

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

It is an error if there is no order in which all additions, functions and references can be resolved due cyclic references!

#### Example Broken Input Files

Cyclic references are an error for obvious reasons.

```
a = (b)
b = (a)
```

It is also an error when a reference can't be resolved due to the first component of the reference "anchoring" the lookup to the "wrong" place.
Consider the following example:

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

The lookup of `a.b` starts by attempting to find `a` inside of `a.a`.
This fails because there is only `c` within `a.a`, not another `a` that could be matched to the `a` in `a.b`.
Next the lookup steps outside of `a.a` and attempts to find the `a` from `a.b` within `a`.
This succeeds, anchoring the lookup of `a.b` to `a.a`.
More precisely, the second component of `a.a` is matched with the first component of `a.b`.
Now the example generates an error because there is no `b` within `a.a`.

The search is not continued inside the top-level `a` because once the lookup is anchored no backtracking is performed.
This is similar to the name lookup rules in the C++ programming language.


### Nested References

References can be arbitrarily **nested**, that is parts of a reference can themselves be a reference!
For this to work the "inner" reference must refer to a value that can natuarlly occur as part of a reference, namely a string or an integer.

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

### References vs. Temporary

When referencing values marked as [temporary](Member-Extensions.md#temporary) the copied value will **not** be marked as [temporary](#temporaries), however any sub-values will retain their temporary status.

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


## Functions

This library contains [a set of config functions](All-Config-Functions.md).
These functions can be used in config files to perform operations on values.
Applications can also add custom functions.

Functions are syntactically similar to references in taoCONFIG, and to lists or function calls in Scheme and LISP.
A function call has one or more arguments, wherefore `(foo)` is parsed as a reference rather than a call to a nullary function.

For example the following config file assigns the value of the environment (shell) variable `$USER` to the config key `foo`.

```
foo = (env "USER")
```

Functions can occur anywhere a value can occur.
A function's arguments can be literal values, other function calls, additions, references, or any combination or nesting of these.

The next example does the same as the previous one, albeit in a slightly contrived manner that shows a composed function argument with reference.

```
(temporary bar)

bar = "ER"
foo = (env "US" + (bar))
```

The config functions supplied with this library are listed and explained on [a separate page](All-Config-Functions.md).

To add custom functions to a config parser and make them available to your config files please consult `src/test/config/custom.cpp` or contact the developers of this library.


## Dotted Names

Wherever the name of an object member is expected it is also possible to use a name with multiple dot-separated components.

Dotted names can contain multiple kinds of components.

1.  Strings, intended to index objects.
2.  Unsigned integers, intended to index arrays.
4.  The [asterisk](#asterisks), intended to index objects or arrays.

The asterisk indexes into all object members or array elements.

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

Dotted names can be thought of as strongly-typed JSON Pointers; there is no ambiguity about whether an integer is intended to index an array or an object.


## Include Files

The `include` feature reads the named file relative to the current working directory and parses it "as if" the contents of that file were present instead of the `include`.
For plain `include` it is an error when the file does not exist, the `include?` alternative form silently ignores this case.

The filename has to be supplied as non-empty string in double quotes and can contain arbitrary characters except for the double quote itself.
Consequently, and unlike all other quoted strings in this library, *no escape sequences* are recognised.

Files can be included anywhere an object member can be defined.
The top-level definitions of the included file will appear in the same place of the JSON hierarchy as the `include`.

#### Example taoCONFIG Input File

```
// Include the file whose contents are shown below.
(include "tests/doc_include.inc")

foo
{
    // Include the same file again, this time within an object.
    (include "tests/doc_include.inc")
}

// Use include? with a non-existing file, not an error.
(include? "tests/non_existing_file_is_no_error_with_include?")
```

#### Example taoCONFIG Include File

```
bar = 42
baz = [ true, false ]
```

#### Resulting JAXN Config Data

```javascript
{
   bar: 42,
   baz: [
      true,
      false
   ],
   foo: {
      bar: 42,
      baz: [
         true,
         false
      ]
   }
}
```


## Temporaries

Sometimes parts of the configuration that are required during config file parsing, for example in order to be [referenced](#references), are not themselves considered part of the actual config.
In these cases the `temporary` feature can be used to exclude any part of the JSON hierarchy from the final config parsing result.

Like `include` the `temporary` statement can occur anywhere an object member can be defined.
It takes a single config key, which can include [asterisks](#asterisks) and be any [dotted name](#dotted-names), as argument.

The argument is always interpreted relative to the position of the `temporary` in the object hierarchy.
For example a `(temporary c.d)` within an object at `a.b` would mark `a.b.c.d` as temporary, independent of whether that node exists (yet).

#### Example taoCONFIG Input File

```
(temporary template)  // Can occur before...

template
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

(temporary template)  // ...and/or after.

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

Similarly the `permanent` feature can be used to restore any part of the JSON hierarchy that was marked as `temporary` to the default behaviour of being part of the final result data.



# Advanced Considerations

For technical reasons reading a configuration is performed in multiple steps.
Authors of config files need to be aware of the two main phases in order to fully understand the semantics of their configs.

### Phase One

The first phase does the reading and parsing of all configuration files.

During this phase all assignments, additions and deletions are recorded in their order of occurrence.
The same goes for included files, their assignments, additions and deletions are performed when and where the include was encountered during parsing.

### Phase Two

The second phase successively eliminates all functions, references and additions.

During this phase the order in which things were parsed is no longer relevant.

 * Functions are eliminated as soon as their arguments are "plain" values, i.e. they are free from functions, references and additions, by replacing the function with the result of calling said function.
 * References are eliminated as soon as the referenced value is "plain", by replacing the reference with a (deep) copy of the referenced value.
 * Additions are eliminated as soon as both operands are "plain", again by replacing the two operands with the result of the addition (which can be a concatenation, or, for objects, a merge).

### Why Phases?

The semantics of `delete`, `+=` and `=`, and, by extension, `include`, depends on the order in which they are processed.
This is due to the combination of features being non-monotonic, which requires linear processing for predictable behaviour.

However for references this would be too limiting as only previously defined values could be referenced, and delta[^1] config files might not be able to make the required modifications.
With references that can "point" both forwards and backwards this problem does not arise.

#### Example taoCONFIG Input File

```
hosts
{
    arctic
    {
        foo = 42
    }
    mystic
    {
        foo = 23
    }
}
host = (env "HOST")

(temporary host)
(temporary hosts)

config
{
    mode = "FOBBLE"
    size = (hosts.(host).foo)
}

(include? "test_delta_config.cfg")
```

This example shows a configuration with parameters that depend on the name of the host machine.

#### Example taoCONFIG Delta File

```
hosts.test01.foo = 3
```

If references were resolved when encountered then the delta config would have no effect -- the `foo` value for the additional host `test01` would be available only *after* the refernce for `config.size` was resolved.

Luckily this is not how taoCONFIG works, the references are resolved during the second phase, and everything will work as expected.



Copyright (c) 2018-2024 Dr. Colin Hirsch and Daniel Frey

[^1]: A "delta" config is typically a configuration file that is optionally included at the end of a full configuration file. The full configuration file usually configures an application for an operational environment; the delta config can optionally modify the full configuration, e.g. for a test environment that might be funcionally identical to the operational environment but have minor differences like using different hostnames.

[CBOR]: http://cbor.io
[JAXN]: https://github.com/stand-art/jaxn
[JSON]: https://tools.ietf.org/html/rfc8259
[MsgPack]: http://msgpack.org
[taoCONFIG]: https://github.com/taocpp/config
[taoJSON]: https://github.com/taocpp/json
[UBJSON]: http://ubjson.org
