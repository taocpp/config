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
In particular, a first line starting with the traditional "shebang", i.e. `#!`, is possible.

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

### Temporary

Copyright (c) 2018 Dr. Colin Hirsch and Daniel Frey
