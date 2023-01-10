// Copyright (c) 2019-2023 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_SCHEMA_INTERNAL_VALIDATOR_HPP
#define TAO_CONFIG_SCHEMA_INTERNAL_VALIDATOR_HPP

#include "../../from_input.hpp"
#include "../validator.hpp"

namespace tao::config::schema::internal
{
   // clang-format off
   const tao::config::schema::validator validator( config::from_input( pegtl_input_t( R"(

      // the schema for schemas:

      definitions
      {
          ref.switch.type
          {
              case.boolean: true
              case.string: "std.key"
              case.array: "ref_list"
              case.object: "schema"
          }

          ref_list: { min_size: 1, items: "ref" }

          type_cases
          {
              properties.optional
              {
                  case.property_names.enum: [ "null", "boolean", "string", "binary", "number", "array", "object" ]
                  case.properties.additional: "ref"
                  default: "ref"
              }
          }

          string_cases
          {
              size: 1
              properties.additional
              {
                  properties.optional
                  {
                      case.properties.additional: "ref"
                      default: "ref"
                  }
              }
          }


          schema
          {
              properties.optional
              {
                  definitions
                  {
                      property_names: "std.identifier"
                      properties.additional: "ref"
                  }

                  // structural
                  type: "ref"
                  not: "ref"

                  all_of: "ref_list"  // short-circuit
                  any_of: "ref_list"  // short-circuit
                  one_of: "ref_list"  // exactly one

                  if: "ref"
                  then: "ref"
                  else: "ref"

                  // any
                  value: true
                  enum.items: true

                  // string/binary/array/object
                  size: "std.unsigned"
                  min_size: "std.unsigned"
                  max_size: "std.unsigned"

                  // string
                  istring.switch.type
                  {
                      case.string: true
                      case.array.items: "string"
                  }
                  pattern: "std.regex"

                  // number
                  minimum: "number"
                  maximum: "number"
                  exclusive_minimum: "number"
                  exclusive_maximum: "number"
                  multiple_of.exclusive_minimum: 0

                  // array
                  items: "ref"
                  unique_items: "boolean"

                  // object
                  property_names: "ref"
                  has_property
                  {
                      case.string: true
                      case.array.items: "string"
                  }
                  property
                  {
                      size: 1
                      properties.additional: "ref"
                  }
                  properties.properties.optional
                  {
                      required.properties.additional: "ref"
                      optional.properties.additional: "ref"
                      additional: "ref"
                  }

                  // special
                  switch {
                      size: 1
                      one_of
                      [
                          { if.has_property: "type" then.property.type: "type_cases" else: false }
                          { if.has_property: "string" then.property.string: "string_cases" else: false }
                          { if.has_property: "istring" then.property.istring: "string_cases" else: false }
                      ]
                  }
              }

              definitions
              {
                  has_size: [
                      { property.type.enum: [ "string", "binary", "array", "object" ] }
                      { has_property: [ "size", "min_size", "max_size" ] }
                  ]
                  is_string: [
                      { property.type.value: "string" }
                      { has_property: [ "istring", "pattern" ] }
                  ]
                  is_number: [
                      { property.type.value: "number" }
                      { has_property: [ "minimum", "maximum", "exclusive_minimum", "exclusive_maximum", "multiple_of" ] }
                  ]
                  is_array: [
                      { property.type.value: "array" }
                      { has_property: [ "items", "unique_items" ] }
                  ]
                  is_object: [
                      { property.type.value: "object" }
                      { has_property: [ "property_names", "has_property", "property", "properties" ] }
                  ]
              }

              all_of
              [
                  { if.has_property: "value" then.properties.optional: { value: true, definitions: true } }
                  { if.has_property: "if" then.has_property: "then" else.not.has_property: "then" }
                  { if.has_property: "else" then.has_property: "then" }

                  {
                      if.property.type.enum: [ "null", "boolean", "binary" ]
                      then.not: [ "schema.is_string", "schema.is_number", "schema.is_array", "schema.is_object" ]
                  }

                  { if: "schema.has_size" then.not: [ { property.type.enum: [ "null", "boolean" ] }, "schema.is_number" ] }

                  { if: "schema.is_string" then.not: [ "schema.is_number", "schema.is_array", "schema.is_object" ] }
                  { if: "schema.is_number" then.not: [ "schema.is_array", "schema.is_object" ] }
                  { if: "schema.is_array" then.not: "schema.is_object" }

                  { if.has_property: "size" then.not.has_property: [ "min_size", "max_size" ] }
                  { if.has_property: "minimum" then.not.has_property: "exclusive_minimum" }
                  { if.has_property: "maximum" then.not.has_property: "exclusive_maximum" }
              ]
          }
      }

      type: "schema"

   )", "schema" ) ) );
   // clang-format on

}  // namespace tao::config::schema::internal

#endif
