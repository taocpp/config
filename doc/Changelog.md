# Changelog

## 1.0.0

**Not yet released**

## Milestones

Pre-1.0.0 milestones in rough reverse chronological order.

* Restrict some phase 1 features[^1].
* Make functions[^2] a phase 2 features.
* Remove schema support (temporarily?).
* Reduce member extensions to the essentials.
* Remove the minus token from config keys.
* Change the semantics of star and move evaluation to phase two.
* Refactor everything in order to simplify the implementation.
* Syntactically turn `delete` into a literal pseudo-value.
* More intuitive semantics requires less knowledge of implementation details.
* Change the semantics to perform nested additions by default.
* Get the interactions between references and additions correct for both arrays and objects.

## History

Development of taoCONFIG started in September 2018 to provide a more expressive config file syntax for [JSON] (or [JAXN]) config files.



Copyright (c) 2018-2024 Dr. Colin Hirsch and Daniel Frey

[^1]: Previously "member extensions".
[^2]: Previously "value extensions".
[JAXN]: https://github.com/stand-art/jaxn
[JSON]: https://tools.ietf.org/html/rfc8259
