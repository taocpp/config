# The Art of C++ / Config

[The Art of C++](https://taocpp.github.io/) / Config is a C++17 header-only library that reads config files based on [JSON](https://tools.ietf.org/html/rfc8259) and [JAXN](https://github.com/stand-art/jaxn) and produces a single [JSON Value](https://github.com/taocpp/json) as result.

## Features

 * JAXN syntax with extensions (compatible with JSON)
 * JAXN data model (JSON + binary data + non-finites)
 * Meta data (filename and position information everywhere)
 * Copy/reference/overwrite values and entire sections
 * Multiple ways to read and parse other files, and use
 * environment variables and output of shell commands.

## Status

This library is still very much under development and not ready for general use.

That said, the features that are implemented are generally functional and tested...

Currently only compiles with our [JSON library](https://github.com/taocpp/json) checked out next to it.

## Example

```
#! Shell-style and C/C++-style comments are supported.

// We are implicitly in a JSON object, the top-level
// enclosing { and } are optional, let's go:

"foo" : 42  // Like in JSON.

"foo" = 42  // '=' is an alternative for ':'.

'foo' = 42  // Keys can be single-quoted, or

foo = 42    // 'simple' keys can be written without quotes.

// As seen above, overwriting existing values is fine!

bar = "first\nsecond"  // A JSON string.

bar = 'first\nsecond'  // Single-quoted alternative.

bar = """first
second"""  // Multi-line string, no escape sequences.

bar = '''
first
second'''  // Newline directly after any triple quotes is ignored.

// Commas are optional, not just on top-level.

commas = {
    arrays = [
	[ 1 2 3 ],
	[ 1, 2, 3 ],
	[ 1, 2, 3, ]
    ]
    objects = [
	{ a : 1 b : 2 c : 3 },
	{ a : 1, b : 2, c : 3 },
	{ a : 1, b : 2, c : 3, }
    ]
}

// Concatenation or addition of values.

addition1 = 1 + 2 + 3
addition2 = true + false

concatenation1 = "Hello, " + "World"
concatenation1 = "Hello, " + (env "USER")  // Let's greet $USER!

concatenation2 = [ 1 2 3 ] + [ 4 5 6 ]

// Nulls are ignored in an addition or concatenation with other values.

null0 = null  // Yields null.
null1 = null + null  // Yields null.
null2 = null + true  // Yields true.
null3 = "ciao" + null  // Yields "ciao".
null4 = [ 1, 2 + null ] + null + [ 3 ]  // Yields [ 1 2 3 ].

// Assignments to nested keys.

a.b.c.d = true  // Creates intermediate objects as required.

// The '-' in an assignment appends a new array element.

d.e.-.- = false  // Also creates the arrays as required.

// Accessing array elements by index requires them to exist.

d.e.0 = true  // Changes d.e from [ [ false ] ] to [ true ].

// References that work both forwards and backwards.

orig1 = 1
ref1 = (orig1)

ref2 = (orig2)
orig2 = 2

(temporary orig1)  // The origs are just for referencing, tag
(temporary orig2)  // them for removal from the final result.

// References can also be nested (without creating cycles).

orig3.a.b.c = 3
i = "b"
j = "i"
o = "orig3"
ref3 = ((o).a.((j)).c)  // Yields the 3 from orig3.a.b.c.

// When a key is 'complex' it can also be quoted in references.

"1 2 3".yes = true

ref4 = ("1 2 3".yes)

// References can also index array positions, or the last element.

orig5 = [ 0 1 2 3 4 5 ]

ref5.a = (orig5.3)  // Zero-based indexing yields a 3.
ref5.b = (orig5.-)  // Yields the last element, here a 5.

// Mixing some of these features together.

orig6 = { a : [ { b : [ [ 0 ] [ 1 2 3 ] [ 2 ] ] } ] }

ref6 = (orig6.a.-.b.1) + [ 4 5 6 ] // Yields [ 1 2 3 4 5 6 ]

/*
   Key Extensions
   - delete       TODO
   - delete?      TODO
   - include      TODO
   - include?     TODO
   - stderr       TODO
   - temporary
*/

/*
   Value Extensions
   - copy         TODO
   - debug        TODO
   - env
   - shell        TODO
   - read         TODO
   - read env     TODO
   - json         TODO
   - json env     TODO
   - json shell   TODO
   - parse        TODO
   - parse env    TODO
   - parse shell  TODO
   - cbor, jaxn, msgpack, ubjson  TODO
   - reference
*/

// TODO: Be continued...
```

See `tests/showcase_only_data.jaxn` for the exact result of parsing this config file example!

## License

The Art of C++ / Config is certified [Open Source] software.
It may be used for any purpose, including commercial purposes, at absolutely no cost.
It is distributed under the terms of the [MIT license] reproduced here.

> Copyright (c) 2018 Dr. Colin Hirsch and Daniel Frey
>
> Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
>
> The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
>
> THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

## Contact

The Art of C++ / Config is part of [The Art of C++](https://taocpp.github.io/).

For questions and suggestions about The Art of C++ / Config please contact the authors at `taocpp(at)icemx.net`.
