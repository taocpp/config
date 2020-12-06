# Member Extensions

*Warning: The documentation is still work-in-progress and very incomplete.*

---

Member extensions use the same syntax as value extensions, however they take the place of JSON object members.
Note that whitespace is significant within member extensions, i.e. whitespace must be used as shown and comments are forbidden.

 * [delete](#delete)
 * [include](#include)
 * [schema](#schema)
 * [setenv](#setenv)
 * [stderr](#stderr)
 * [temporary](#temporary)


## delete

The `delete` member extension is deprecated and the `delete` special value should be used instead, TODO: link.

The `delete` member extensions delete a value (and all of its sub-values) from the config.
For plain `delete` it is an error if the value does not exist, the `delete?` alternative form is idempotent and never reports an error.
Values that were deleted can be assigned to again later.

#### Example taoCONFIG Input File

```
foo = "LG"
bar = "RL"

(delete foo)
(delete? baz)
```

#### Resulting JAXN Config Data

```javascript
{
   bar: "RL"
}
```


## include


## schema

The `schema` member extension tells the config library which [schema file](Writing-Schema-Files.cfg) the config must adhere to.
After reading the config file(s), the schema file from the last `schema` directive, if any, is read, and the config is checked against it.

#### Example taoCONFIG Input File

```
ip = "127.0.0.1"
port = 42

(schema "tests/doc_member_schema.schema")
```

#### Example taoCONFIG Schema File

```
definitions
{
    port
    {
        type: "std.unsigned"
        minimum: 1
        maximum: 65535
    }
}

properties
{
    required
    {
        ip: "std.net.ip_v4_address"
        port: "port"
    }
}
```

#### Resulting Printed Debug Data

```javascript
{
   ip: "127.0.0.1",
   port: 42
}
```

For more on schemas [please consult the page on how to write schema files](Writing-Schema-Files.md).


## setenv

*The `setenv` member extension only exists to make the examples independent of their environment.*


## stderr

This member extension is a debugging tool and is similar to the `debug` value extension.

It prints a JSON representation of the library's internal intermediate data structure for the referenced part of the config data in its current state to the standard error channel.
The config value itself does not change through the presence or absence of "stderr members".

#### Example taoCONFIG Input File

```
foo = 42

(stderr foo)
```

#### Resulting Printed Debug Data

```javascript
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

#### Resulting JAXN Config Data

```javascript
{
   foo: 42
}
```


## temporary

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

#### Resulting JAXN Config Data

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

[CBOR]: http://cbor.io
[JAXN]: https://github.com/stand-art/jaxn
[JSON]: https://tools.ietf.org/html/rfc8259
[MsgPack]: http://msgpack.org
[taoCONFIG]: https://github.com/taocpp/config
[taoJSON]: https://github.com/taocpp/json
[UBJSON]: http://ubjson.org
