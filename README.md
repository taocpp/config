# The Art of C++ / Config

[The Art of C++] / Config is a C++ header-only library that reads config files based on [JSON] and [JAXN] and produces a single [JSON Value] as result.

## Documentation

*Warning: The library and documentation are still work-in-progress and not complete.*

 * [Version 1.x](doc/README.md) (requires C++17)

## Features

 * [JAXN] syntax with extensions (backward compatible with [JSON]).
 * [JAXN] data model ([JSON] extended with binary data and non-finites).
 * Meta data, all sub-values are annotated with filename and position.
 * Copy, reference, replace and delete anything in the [JSON] structure.
 * Multiple ways to read and parse other config and data files, and
 * use environment variables and the output of arbitrary shell commands.
 * The function [`tao::config::parse_file()`](doc/Parsing-Config-Files.md) is all you need to get going.

## Status

This library is still under heavy development and not very well documented...

...however it generally works, and test coverage and error messages are not too bad.

To compile the tests and examples it requires [taocpp/json] checked out next to it.

## Example

```
#! Shell-style and C/C++-style comments are supported.

// The syntax is backwards compatible with both JSON
// and JAXN, so every JSON file is a valid config file
// (as long as its top-level value is an object), but
// the top-level '{' and '}' are optional.

// JSON object members.

"foo": 42   // Like in JSON.

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
second'''  // Newline directly after an opening triple quote is ignored.

# Commas are optional, not just on top-level.

commas = {
    arrays = [
	[ 1 2 3 ],
	[ 1, 2, 3 ],
	[ 1, 2, 3, ] # trailing commas are allowed
    ]
    objects = [
	{ a: 1 b: 2 c: 3 },
	{ a: 1, b: 2, c: 3 },
	{ a: 1, b: 2, c: 3, }
    ]
}

// Concatenation or addition of values.

addition1 = 1 + 2 + 3
addition2 = true + false

// Note that integers and doubles cannot be mixed in additions.

concatenation1 = "Hello, " + "World"
concatenation2 = [ 1 2 3 ] + [ 4 5 6 ]  // Yields [ 1 2 3 4 5 6 ]

// Extending existing values.

concatenation2 += [ 7 8 9 ]  // Yields [ 1 2 3 4 5 6 7 8 9 ]

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

# Accessing array elements by index requires them to exist.

d.e.0 = true  // Changes d.e from [ [ false ] ] to [ true ].

// References that work both forwards and backwards.

orig1 = 1
ref1 = (orig1)

ref2 = (orig2)
orig2 = 2

(temporary orig1)  // The origs are just for referencing, tag
(temporary orig2)  // them for removal from the final result.

// References can also be nested arbitrarily.

orig3.a.b.c = "x"
i = "b"
j = "i"
o = "orig3"
ref3 = ((o).a.((j)).c)  // Yields the "x" from orig3.a.b.c.

// When a key is 'complex' or starts with a digit it can also be quoted in references.

"1 2 3".yes = true

ref4 = ("1 2 3".yes)

// References can also index array positions, or the last element.

orig5 = [ 10 11 12 13 14 15 ]

ref5.a = (orig5.3)  // Zero-based indexing yields a 13.
ref5.b = (orig5.-)  // Yields the last element, here a 15.

// Mixing some of these features together.

orig6 = { a: [ { b: [ [ 0 ] [ 1 2 3 ] [ 2 ] ] } ] }

ref6 = (orig6.a.-.b.1) + [ 4 5 6 ] // Yields [ 1 2 3 4 5 6 ]

// Copying values and sections.

copy1s = { a: { b: 100, c: 200 } }  // Same as copy1s.a.b = 100 and copy1s.a.c = 200 when copy1s wasn't previously set.

copy1d = (copy copy1s.a)  // copy1d is now a DEEP COPY of copy1s.a

copy1s.a.b = 300  // Does NOT change the value of copy1d.b from 100 to 300!

// Copying can also use - to access the last array element.

copy2s = [ [ "foo" "bar" "baz" ] ]

copy2d1 = (copy copy2s.-.0)  // "foo"
copy2d2 = (copy copy2s.-.-)  // "baz"

// Deleting values and sections.

del1 = 1

(delete del1)  // Deletes the corresponding top-level member.

del2.a.b.c = 1

(delete del2)  // Same thing, removes del2 and everything it contains.

del3 = [ 1 2 3 4 5 ]

/* Array indices start at 0 -- TODO: Change to 1? */

(delete del3.2)  // Deletes the third element of the array.

del4 = [ 0 1 2 ] + [ 3 4 5 ]

(delete del4.4)  // Deletes the fifth element which is the 4.

del5.a.b = {
   c = 1
   d = 2
}

(delete del5.a.b.*)  // Deletes all members from the object del5.a.b.

del6.a.b = [ 1 2 3 ]

(delete del6.a.b.*)  // Deletes all members from the array del6.a.b.

del7 = [
   { a = 1, b = 2 }
   { a = 2, c = 3 }
]

(delete del7.-.a)  // Delete a from the last element of the array del7.

del8 = {}

(delete? del8.a)  // No error if it there is nothing to delete.

// Value extensions.

value1 = (debug orig6.a)  // The intermediate data structure as string.

value2 = (env "VARIABLE")  // Contents of environment variable $VARIABLE as string.

value3 = (env? "NONEXISTENT" "default")  // Use default for missing variable.

value4 = (shell "echo hallo")  // The output of the shell command as string.

value5 = (read "tests/showcase.t")  // The contents of the file as string.

value6 = (read (env "TEXTFILE"))  // Like above but uses $TEXTFILE as filename.

value7 = (json (read "tests/showcase.j"))  // The contents of the file as JSON value.

value8 = (json (read (env "JSONFILE")))  // Like above but uses $JSONFILE as filename.

value9 = (json (shell '''echo "[]"'''))  // Parses the output of the shell as JSON.

// Note that the previous example uses triple-single-quotes for a multi-line string.

// Note that (json ...) also works for cbor, jaxn, msgpack and ubjson.

value10 = (parse (read "tests/showcase.c"))  // Parses a single value with config syntax.

value11 = (parse (read (env "CONFIGFILE")))  // Like above but uses $CONFIGFILE as filename.

value12 = (parse (shell "echo true"))  // Parses the outpuf of the shell as config value.

// Note that value extensions can not be part of references or vice versa, however
// value extensions and references can both be part of an addition or concatenation.

v.w.- = "something"

value13 = (env "VARIABLE") + (v.w.0)  // This kind of mixing is ok.

// Including other (complete) config files.

(include "tests/simple.config")  // Reads other file "as if" it were part of this one.

inc.nested.test = {
   (include "tests/simple.config")  // Same as above, but parses into inc.nested.test.
}

/*
   With simple.config containing one assignment, simple = 42, we now have
   * simple = 42 at top-level from the first include, and
   * inc.nested.test.simple = 42 from the second include.
*/

(include? "/___this_does_not_exist.txt___")  // No error when the file can't be read.

# The include without the question mark throws an exception when the file can't be read.

(include? (env? "FROBNICATE"))  // No error when the env lookup fails or the file can't be read.

# Both forms of include normally report syntax errors etc. while parsing the files.

// Different styles for the same data.

server.local.ip = "127.0.0.1"
server.local.ports.- = 80
server.local.ports.- = 443
server.local.ports.- = 8080

server = {
   local = {
      ip: "127.0.0.1"
      ports: [ 80, 443, 8080 ]
   }
}
```

See `tests/showcase_only_data.jaxn` for the [JSON] (actually [JAXN]) data that results from parsing this example config.

## Contact

The Art of C++ / Config is part of [The Art of C++].

For questions and suggestions about The Art of C++ / Config please contact the authors at `taocpp(at)icemx.net`.

## License

The Art of C++ / Config is certified [Open Source] software.
It may be used for any purpose, including commercial purposes, at absolutely no cost.
It is distributed under the terms of the [MIT license] reproduced here.

> Copyright (c) 2018-2020 Dr. Colin Hirsch and Daniel Frey
>
> Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
>
> The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
>
> THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

[JAXN]: https://github.com/stand-art/jaxn/
[JSON]: https://tools.ietf.org/html/rfc8259
[JSON Value]: https://github.com/taocpp/json/
[MIT license]: http://www.opensource.org/licenses/mit-license.html
[Open Source]: http://www.opensource.org/docs/definition.html
[taocpp/json]: https://github.com/taocpp/json/
[The Art of C++]: https://taocpp.github.io/
