# Member Extensions

Member extensions use the same syntax as value extensions, however they take the place of JSON object members.
Note that whitespace is significant within member extensions, i.e. whitespace must be used as shown and comments are forbidden.

 * [include](#include)
 * [parse](#parse)
 * [permanent](#permanent)
 * [schema](#schema)
 * [temporary](#temporary)



## include

The `include` member extension reads the named file relative to the current working directory and parses it "as if" the contents of that file were present instead of the `include`.
For plain `include` it is an error when the file does not exist, the `include?` alternative form ignores missing files.

#### Example taoCONFIG Input File

```
// Include the file whose contents are shown below.
(include "tests/doc_member_include.inc")

foo
{
    // Include the same file again, this time within an object.
    (include "tests/doc_member_include.inc")
}

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



## parse

The `parse` member extension parses the given string as "as if" the contents of that string were present instead of the `parse`.

#### Example taoCONFIG Input File

```
(parse "a = 42")

foo
{
    b = true
    c = false
    (parse "b = delete   c = true")
}
```

#### Resulting JAXN Config Data

```javascript
{
   a: 42,
   foo: {
      c: true
   }
}
```



## permanent

The `permanent` member extension is used to undo the effects of [`temporary`](#temporary).



## schema

The `schema` member extension tells the config library which [schema file](Writing-Schema-Files.cfg) the config must adhere to.
After reading the config file(s), the schema is read, and the config, more precisely: the config (sub-)object of which the schema was set, is checked against it.

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

#### Resulting JAXN Config Data

```javascript
{
   ip: "127.0.0.1",
   port: 42
}
```

For more information on taoCONFIG schemas [please consult the page on how to write schema files](Writing-Schema-Files.md).

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

#### Resulting JAXN Config Data

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



## temporary

The `temporary` member extension marks a sub-tree of the JSON as temporary, meaning that it will be removed from the final resulting JSON value.
This is frequently used for templates that are referenced somewhere else in the config.

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



Copyright (c) 2018-2024 Dr. Colin Hirsch and Daniel Frey

[JAXN]: https://github.com/stand-art/jaxn
[JSON]: https://tools.ietf.org/html/rfc8259
[taoCONFIG]: https://github.com/taocpp/config
[taoJSON]: https://github.com/taocpp/json
