# Writing Config Files

*Warning: The documentation is still work-in-progress and very incomplete.*

The config file syntax is based on JSON and JAXN, in fact it is completly backward compatible.
Every JSON or JAXN file with a top-level JSON Object is also a valid taocpp/config file.

 * [Overview](#overview)
 * [Comments](#comments)
 * [State Model](#state-model)
 * [Member Keys](#member-keys)
 * [Assignments](#assignments)
 * [Additions](#additions)
 * [Simple Values](#simple-values)
 * [Number Values](#number-values)
 * [String Values](#string-values)
 * [Binary Values](#binary-values)
 * [References](#references)
 * [Value Extensions](#value-extensions)
 * [Member Extensions](#member-extensions)

## Overview

## Comments

The same kinds of comments as in JAXN are supported, "shell-style" until-end-of-line with `#`, "C++-style" until-end-of-line with `//`, and "C-style" non-nesting block comments with `/*` and `*/`.
In particular, a first line starting with the traditional "shebang", i.e. `#!`, can be used.

## State Model

## Assignments

## Additions

## Simple Values

## Number Values

## String Values

## Binary Values

## References

## Value Extensions

Extensions are features written in round brackets.

### Copy

### Debug

### Env

### Json

All other file formats supported by taocpp/json can be parsed this way using `cbor`, `jaxn`, `msgpack` and `ubjson` instead of `json` as command name.

### Parse

### Read

### Shell

## Member Extensions

Member extensions use the same syntax as value extensions, however they take the place of JSON Object members.

### Delete

### Include

### Stderr

This member extension is a debugging tool.
It prints a JSON representation of the library's internal intermediate data structure to the standard error channel.
The config value itself does not change through the presence or absence of "stderr members".

#### Example taoCONFIG Input File

```
foo = 42

(stderr foo)
```

#### Resulting Printed Debug Data

```
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

#### Resulting JSON/JAXN Config Data

```
{
   foo: 42
}
```

### Temporary

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

#### Resulting JSON/JAXN Config Data

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
