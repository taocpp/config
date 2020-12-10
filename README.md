# The Art of C++ / Config

[The Art of C++] / Config is a C++ header-only library that reads config files based on [JSON] and [JAXN] and produces a single [JSON Value] as result.

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
