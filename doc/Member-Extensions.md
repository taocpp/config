# Member Extensions

---

Member extensions use the same syntax as value extensions, however they take the place of JSON object members.
Note that whitespace is significant within member extensions, i.e. whitespace must be used as shown and comments are forbidden.

 * [include](#include)
 * [parse](#parse)
 * [schema](#schema)



## include



## parse



## schema

The `schema` member extension tells the config library which [schema file](Writing-Schema-Files.cfg) the config must adhere to.
After reading the config file(s), the schema is read, and the config is checked against it.

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

Schemas can also be applied to sub-sections of the config by putting the schema member in the appropriate sub-section.

#### Example taoCONFIG Input File

```
foo.bar
{
    ip = "127.0.0.1"
    port = 42

    (schema "tests/doc_member_schema.schema")
}

what = "ever"  // Outside scope of schema.
```

#### Resulting Printed Debug Data

```javascript
{
   foo: {
      bar: {
         ip: "127.0.0.1",
         port: 42
      }
   },
   what: "ever"
}
```

The schema member can occur multiple times at top-level or within a sub-section, however the last one "wins" and only a single schema will actually be used.
In order to "cancel" a schema checking the schema member can be used with `null`.

```
(schema null)  // Remove any top-level schema checking.
```



Copyright (c) 2018-2021 Dr. Colin Hirsch and Daniel Frey

[JAXN]: https://github.com/stand-art/jaxn
[JSON]: https://tools.ietf.org/html/rfc8259
[taoCONFIG]: https://github.com/taocpp/config
[taoJSON]: https://github.com/taocpp/json
