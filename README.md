# The Art of C++ / Config

[![Windows CI](https://github.com/taocpp/config/workflows/Windows/badge.svg)](https://github.com/taocpp/config/actions?query=workflow%3AWindows)
[![macOS CI](https://github.com/taocpp/config/workflows/macOS/badge.svg)](https://github.com/taocpp/config/actions?query=workflow%3AmacOS)
[![Linux CI](https://github.com/taocpp/config/workflows/Linux/badge.svg)](https://github.com/taocpp/config/actions?query=workflow%3ALinux)
<br>
[![clang-analyze](https://github.com/taocpp/config/workflows/clang-analyze/badge.svg)](https://github.com/taocpp/config/actions?query=workflow%3Aclang-analyze)
[![clang-tidy](https://github.com/taocpp/config/workflows/clang-tidy/badge.svg)](https://github.com/taocpp/config/actions?query=workflow%3Aclang-tidy)
[![Sanitizer](https://github.com/taocpp/config/workflows/Sanitizer/badge.svg)](https://github.com/taocpp/config/actions?query=workflow%3ASanitizer)
[![CodeQL](https://github.com/taocpp/config/workflows/CodeQL/badge.svg)](https://github.com/taocpp/config/actions?query=workflow%3ACodeQL)
[![Code Coverage](https://codecov.io/gh/taocpp/config/branch/master/graph/badge.svg?token=ykWa8RRdyk)](https://codecov.io/gh/taocpp/config)

[The Art of C++] / Config is a C++ header-only library that reads config files in a format based on [JSON] and [JAXN] and produces a single [JSON Value] as result.

## Documentation

 * [Changelog](doc/Changelog.md)
 * [Version 1.x](doc/README.md) (requires C++17)

## Contact

<a href="https://discord.gg/VQYkppcgqN"><img align="right" src="https://discordapp.com/api/guilds/790164930083028993/embed.png?style=banner2" alt="Join us on Discord"></a>

For questions and suggestions regarding The Art of C++ / Config, success or failure stories, and any other kind of feedback, please feel free to join our [Discord](https://discord.gg/VQYkppcgqN) server, open a [discussion](https://github.com/taocpp/config/discussions), an [issue](https://github.com/taocpp/config/issues) or a [pull request](https://github.com/taocpp/config/pulls) on GitHub or contact the authors at `taocpp(at)icemx.net`.

## Introduction

 * [JAXN] syntax with extensions (backward compatible with [JSON]).
 * [JAXN] data model ([JSON] extended with binary data and non-finites).
 * Meta data, all sub-values are annotated with filename and position.
 * Copy, reference, replace and delete anything in the [JSON] structure.
 * Multiple ways to read and include other config and data files, and
 * use environment variables and the output of arbitrary shell commands.
 * The function [`tao::config::from_file()`](doc/Parsing-Config-Files.md) is all you need to get going.

Every JSON file with a top-level object can be used as config file.

```
{
   "ip": "127.0.0.2",
   "port": 27960,
   "maps": [ "ztn", "dm13", "t9" ]
}
```

This small example can be rendered differently using some of the additional syntactic possibilities of the config file format.

```
#!/usr/local/bin/q3s

ip = "127.0.0.2"
port = 27960
maps = [ "ztn" "dm13" "t9" ]  // Add dm6 or t4?
```

Semantic features like deleting and referencing values, or including files and reading environment variables, usually only make sense with larger, non-trivial real-world examples.

These features can be used to manage situations that go beyond single deployments with a single config, for example providing the tools to manage configuration templates that are adapted to different environments.

Parsing a config file generally entails nothing more than calling the appropriate `from_file()` function with the filename.

```
#include <tao/config.hpp>

const tao::config::value config = tao::config::from_file( "foo.cfg" );
```

The resulting value is nothing other but a [JSON Value] from The Art of C++ / JSON with a custom traits class.
It can be inspected using all the facilities of that JSON library.

## License

The Art of C++ / Config is certified [Open Source] software.
It may be used for any purpose, including commercial purposes, at absolutely no cost.
It is distributed under the terms of the [MIT license] reproduced here.

> Copyright (c) 2018-2021 Dr. Colin Hirsch and Daniel Frey
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
