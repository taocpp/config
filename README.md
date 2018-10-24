# The Art of C++ / Config

[The Art of C++](https://taocpp.github.io/) / Config is a C++17 header-only library that reads config files in [JSON](https://tools.ietf.org/html/rfc8259) and [JAXN](https://github.com/stand-art/jaxn) syntax with extensions and produces a single [JSON Value](https://github.com/taocpp/json) as result.

## Features

 * JAXN data model
   * JSON data model
   * Binary data
   * All double values
 * Based on JAXN syntax
   * Based on JSON syntax
   * Commas are optional
   * JAXN comments
   * Other small extensions
 * Phase 1 features
   * Copy sub-sections
   * Include other config files
   * Read other files as value
   * Invoke shell commands
   * Access environment variables
 * Phase 2 features
   * Reference sub-values
   * Value addition or concatenation
 * File position information (TODO)
 * ...

## Example

```
# JAXN comments...

/*
  ...Shell and C and C++-style comments.
*/

// Regular object member
foo : 41

// Alternative syntax
bar = 43

// Overwrite earlier values
foo : 42

// Nested object member
a.b.c.d = 44

// Include other config file
(include "other.config")

// Include into sub-object
a.b.c = {
   (include "other.config")
}

// Add or overwrite a value
a.b.c.d = "hallo"

// Copy sub-section
d.e = (copy a.b.c)

// Value addition
sum = 1 + 2 + 3

// Commas are optional
comma : {
    array : [
	[ 1 2 3 ],
	[ 1, 2, 3 ],
	[ 1, 2, 3, ]
    ]
    object : [
	{ a : 1 b : 2 c : 3 },
	{ a : 1, b : 2, c : 3 },
	{ a : 1, b : 2, c : 3, }
    ]
}

// Array concatenation
array = [ 1 2 3 ] + [ 4 5 6 ]

// Concatenate later
array += [ 7 8 9 ]

// Add new last element to array
array.- = 10

// Shell commands
user = (shell "echo '[ 1 2 3 ]'")

// Environment variables
user = (env "USER")

// Multi-line strings
greeting = """
Hello,
World!
"""

// Nested references

j = 1
a.b.c = 40
i = "b"
j += (a.(i).c) + 1  // now j is 42

// Delete some values

(delete bar)

(delete comma.object.1.*)

// ...more...
```

## Status

This library is still very much under development and not ready for general use.

That said, the features that are implemented are generally functional and stable...

Currently only compiles with our [JSON library](https://github.com/taocpp/json) checked out next to it.

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
