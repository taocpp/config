// Copyright (c) 2018-2024 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_INTERNAL_DEBUG_TRAITS_HPP
#define TAO_CONFIG_INTERNAL_DEBUG_TRAITS_HPP

#include <stdexcept>
#include <string>
#include <vector>

#include <tao/json/contrib/variant_traits.hpp>

#include "array.hpp"
#include "concat.hpp"
#include "entry.hpp"
#include "json.hpp"
#include "key1.hpp"
#include "object.hpp"
#include "pegtl.hpp"
#include "reference2.hpp"

#include "../annotation.hpp"
#include "../key.hpp"

namespace tao::config::internal
{
   template< typename T >
   struct debug_traits
      : json::traits< T >
   {};

   template<>
   struct debug_traits< void >
      : json::traits< void >
   {};

   template<>
   struct debug_traits< key_kind >
   {
      TAO_JSON_DEFAULT_KEY( "key_kind" );

      template< template< typename... > class Traits, typename Consumer >
      static void produce( Consumer& c, const key_kind k )
      {
         switch( k ) {
            case key_kind::name:
               c.string( "name" );
               return;
            case key_kind::index:
               c.string( "index" );
               return;
         }
         throw std::logic_error( "code should be unreachable" );  // LCOV_EXCL_LINE
      }
   };

   template<>
   struct debug_traits< key_part >
   {
      TAO_JSON_DEFAULT_KEY( "key_part" );

      template< template< typename... > class Traits, typename Consumer >
      static void produce( Consumer& c, const key_part& p )
      {
         c.begin_object( 2 );
         c.key( "key_kind" );
         json::events::produce< Traits >( c, p.kind() );
         c.member();
         c.key( "key_data" );
         json::events::produce< Traits >( c, p.data );
         c.member();
         c.end_object( 2 );
      }
   };

   template<>
   struct debug_traits< key >
      : json::traits< std::vector< key_part > >
   {
      TAO_JSON_DEFAULT_KEY( "key" );
   };

   template<>
   struct debug_traits< annotation >
      : json::binding::object< TAO_JSON_BIND_REQUIRED( "key", &annotation::key ),
                               TAO_JSON_BIND_REQUIRED( "position", &annotation::position ) >
   {
      TAO_JSON_DEFAULT_KEY( "annotation" );
   };

   template<>
   struct debug_traits< json::position >
   {
      TAO_JSON_DEFAULT_KEY( "position" );

      template< template< typename... > class Traits, typename Consumer >
      static void produce( Consumer& c, const json::position& p )
      {
         c.string( p.source() + ':' + std::to_string( p.line() ) + ':' + std::to_string( p.column() ) );
      }
   };

   template<>
   struct debug_traits< pegtl::position >
   {
      TAO_JSON_DEFAULT_KEY( "position" );

      template< template< typename... > class Traits, typename Consumer >
      static void produce( Consumer& c, const pegtl::position& p )
      {
         c.string( p.source + ':' + std::to_string( p.line ) + ':' + std::to_string( p.column ) );
      }
   };

   template<>
   struct debug_traits< part_asterisk_t >
   {
      template< template< typename... > class Traits, typename Consumer >
      static void produce( Consumer& c, const part_asterisk_t /*unused*/ )
      {
         c.string( "asterisk" );
      }
   };

   template<>
   struct debug_traits< key1_kind >
   {
      TAO_JSON_DEFAULT_KEY( "key1_kind" );

      template< template< typename... > class Traits, typename Consumer >
      static void produce( Consumer& c, const key1_kind k )
      {
         switch( k ) {
            case key1_kind::name:
               c.string( "name" );
               return;
            case key1_kind::index:
               c.string( "index" );
               return;
            case key1_kind::asterisk:
               c.string( "asterisk" );
               return;
            case key1_kind::append:
               c.string( "append" );
               return;
         }
         throw std::logic_error( "code should be unreachable" );  // LCOV_EXCL_LINE
      }
   };

   template<>
   struct debug_traits< key1_part >
   {
      TAO_JSON_DEFAULT_KEY( "key1_part" );

      template< template< typename... > class Traits, typename Consumer >
      static void produce( Consumer& c, const key1_part& p )
      {
         c.begin_object( 3 );
         c.key( "key1_kind" );
         json::events::produce< Traits >( c, p.kind() );
         c.member();
         c.key( "key1_data" );
         json::events::produce< Traits >( c, p.data );
         c.member();
         c.key( "position" );
         json::events::produce< Traits >( c, p.position );
         c.member();
         c.end_object( 3 );
      }
   };

   template<>
   struct debug_traits< key1 >
      : json::traits< std::vector< key1_part > >
   {
      TAO_JSON_DEFAULT_KEY( "key1" );
   };

   template<>
   struct debug_traits< reference2_kind >
   {
      TAO_JSON_DEFAULT_KEY( "reference2_kind" );

      template< template< typename... > class Traits, typename Consumer >
      static void produce( Consumer& c, const reference2_kind k )
      {
         switch( k ) {
            case reference2_kind::name:
               c.string( "name" );
               return;
            case reference2_kind::index:
               c.string( "index" );
               return;
            case reference2_kind::vector:
               c.string( "reference" );
               return;
         }
         throw std::logic_error( "code should be unreachable" );  // LCOV_EXCL_LINE
      }
   };

   template<>
   struct debug_traits< reference2_part >
   {
      TAO_JSON_DEFAULT_KEY( "reference2_part" );

      template< template< typename... > class Traits, typename Consumer >
      static void produce( Consumer& c, const reference2_part& p )
      {
         c.begin_object( 3 );
         c.key( "reference2_kind" );
         json::events::produce< Traits >( c, p.kind() );
         c.member();
         c.key( "reference2_data" );
         json::events::produce< Traits >( c, p.data );
         c.member();
         c.key( "position" );
         json::events::produce< Traits >( c, p.position );
         c.member();
         c.end_object( 3 );
      }
   };

   template<>
   struct debug_traits< reference2 >
      : json::traits< std::vector< reference2_part > >
   {
      TAO_JSON_DEFAULT_KEY( "reference2" );
   };

   template<>
   struct debug_traits< entry_kind >
   {
      TAO_JSON_DEFAULT_KEY( "entry_kind" );

      template< template< typename... > class Traits, typename Consumer >
      static void produce( Consumer& c, const entry_kind k )
      {
         switch( k ) {
            case entry_kind::NULL_:
               c.string( "null" );
               return;
            case entry_kind::BOOLEAN:
               c.string( "boolean" );
               return;
            case entry_kind::STRING:
               c.string( "string" );
               return;
            case entry_kind::BINARY:
               c.string( "binary" );
               return;
            case entry_kind::SIGNED:
               c.string( "signed" );
               return;
            case entry_kind::UNSIGNED:
               c.string( "unsigned" );
               return;
            case entry_kind::DOUBLE:
               c.string( "double" );
               return;
            case entry_kind::ARRAY:
               c.string( "array" );
               return;
            case entry_kind::OBJECT:
               c.string( "object" );
               return;
            case entry_kind::ASTERISK:
               c.string( "asterisk" );
               return;
            case entry_kind::REFERENCE:
               c.string( "reference" );
               return;
         }
         throw std::logic_error( "code should be unreachable" );  // LCOV_EXCL_LINE
      }
   };

   template<>
   struct debug_traits< entry >
   {
      template< template< typename... > class Traits, typename Consumer >
      static void produce( Consumer& c, const entry& v )
      {
         c.begin_object();
         c.key( "type" );
         json::events::produce< Traits >( c, v.kind() );
         c.member();
         switch( v.kind() ) {
            case entry_kind::NULL_:
               c.end_object();
               return;
            case entry_kind::BOOLEAN:
               c.key( "data" );
               json::events::produce< Traits >( c, v.get_boolean() );
               break;
            case entry_kind::STRING:
               c.key( "data" );
               json::events::produce< Traits >( c, v.get_string() );
               break;
            case entry_kind::BINARY:
               c.key( "data" );
               json::events::produce< Traits >( c, v.get_binary() );
               break;
            case entry_kind::SIGNED:
               c.key( "data" );
               json::events::produce< Traits >( c, v.get_signed() );
               break;
            case entry_kind::UNSIGNED:
               c.key( "data" );
               json::events::produce< Traits >( c, v.get_unsigned() );
               break;
            case entry_kind::DOUBLE:
               c.key( "data" );
               json::events::produce< Traits >( c, v.get_double() );
               break;
            case entry_kind::ARRAY:
               c.key( "data" );
               json::events::produce< Traits >( c, v.get_array() );
               break;
            case entry_kind::OBJECT:
               c.key( "data" );
               json::events::produce< Traits >( c, v.get_object() );
               break;
            case entry_kind::ASTERISK:
               c.key( "data" );
               json::events::produce< Traits >( c, v.get_asterisk() );
               break;
            case entry_kind::REFERENCE:
               c.key( "data" );
               json::events::produce< Traits >( c, v.get_reference() );
               break;
         }
         c.member();
         c.end_object();
      }
   };

   template<>
   struct debug_traits< concat >
      : json::binding::object< TAO_JSON_BIND_REQUIRED( "remove", &concat::remove ),
                               TAO_JSON_BIND_REQUIRED( "implicit", &concat::implicit ),
                               TAO_JSON_BIND_REQUIRED( "temporary", &concat::temporary ),
                               TAO_JSON_BIND_REQUIRED( "position", &concat::position ),
                               TAO_JSON_BIND_REQUIRED( "concat_list", &concat::concat ) >
   {};

   template<>
   struct debug_traits< array >
      : json::binding::object< TAO_JSON_BIND_REQUIRED( "position", &array::position ),
                               TAO_JSON_BIND_OPTIONAL( "function", &array::function ),
                               TAO_JSON_BIND_REQUIRED( "array_data", &array::array ) >
   {};

   template<>
   struct debug_traits< object >
      : json::binding::object< TAO_JSON_BIND_REQUIRED( "position", &object::position ),
                               TAO_JSON_BIND_REQUIRED( "object_data", &object::object ) >
   {};

   template< typename T >
   struct debug_traits< const T* >
   {
      template< template< typename... > class Traits >
      static void assign( json::basic_value< Traits >& v, const T* p )
      {
         v.set_opaque_ptr( p );
      }
   };

   template< typename T >
   struct debug_traits< T* >
      : debug_traits< const T* >
   {};

}  // namespace tao::config::internal

#endif
