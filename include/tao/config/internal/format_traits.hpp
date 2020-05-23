// Copyright (c) 2018-2020 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_INTERNAL_FORMAT_TRAITS_HPP
#define TAO_CONFIG_INTERNAL_FORMAT_TRAITS_HPP

#include "../traits.hpp"

#include "concat.hpp"
#include "entry.hpp"
#include "json.hpp"
#include "value_traits.hpp"

namespace tao::config::internal
{
   template< typename T >
   struct format_traits
      : public config::traits< T >
   {
   };

   template<>
   struct format_traits< void >
      : public json::traits< void >
   {
   };

   template<>
   struct format_traits< pegtl::position >
   {
      template< template< typename... > class Traits, typename Consumer >
      static void produce( Consumer& c, const pegtl::position& p )
      {
         c.string( p.source + ':' + std::to_string( p.line ) + ':' + std::to_string( p.column ) );
      }
   };

   template<>
   struct format_traits< const pegtl::position* >
   {
      TAO_JSON_DEFAULT_KEY( "position" );

      template< template< typename... > class Traits >
      static void assign( json::basic_value< Traits >& v, const pegtl::position* p )
      {
         v.set_opaque_ptr( p );
      }
   };

   template<>
   struct format_traits< pegtl::position* >
      : public format_traits< const pegtl::position* >
   {
   };

   template<>
   struct format_traits< const std::optional< pegtl::position >* >
   {
      TAO_JSON_DEFAULT_KEY( "position" );

      template< template< typename... > class Traits >
      static void assign( json::basic_value< Traits >& v, const std::optional< pegtl::position >* p )
      {
         if( *p ) {
            v.set_opaque_ptr( &**p );
         }
         else {
            v.set_null();
         }
      }
   };

   template<>
   struct format_traits< std::optional< pegtl::position >* >
      : public format_traits< const std::optional< pegtl::position >* >
   {
   };

   template<>
   struct format_traits< json::type >
   {
      TAO_JSON_DEFAULT_KEY( "type" );

      template< template< typename... > class Traits >
      static void assign( json::basic_value< Traits >& v, const json::type t )
      {
         v.emplace_string( json::to_string( t ) );
      }
   };

   template<>
   struct format_traits< json::basic_value< value_traits > >
   {
      template< template< typename... > class, typename Consumer >
      static void produce( Consumer& c, const json::basic_value< value_traits >& v )
      {
         json::events::from_value( c, v );
      }
   };

   template<>
   struct format_traits< const json::basic_value< value_traits >* >
   {
      template< template< typename... > class Traits >
      static void assign( json::basic_value< Traits >& v, const json::basic_value< value_traits >* e )
      {
         v.set_opaque_ptr( e );
      }
   };

   template<>
   struct format_traits< entry::kind >
   {
      TAO_JSON_DEFAULT_KEY( "type" );

      template< template< typename... > class Traits >
      static void assign( json::basic_value< Traits >& v, const entry::kind k )
      {
         switch( k ) {
            case entry::atom:
               v.set_string_view( "atom" );
               return;
            case entry::array:
               v.set_string_view( "array" );
               return;
            case entry::object:
               v.set_string_view( "object" );
               return;
            case entry::nothing:
               v.set_string_view( "nothing" );
               return;
            case entry::reference:
               v.set_string_view( "reference" );
               return;
         }
         assert( false );
      }

      template< template< typename... > class Traits, typename Consumer >
      static void produce( Consumer& c, const entry::kind k )
      {
         switch( k ) {
            case entry::atom:
               c.string( "atom" );
               return;
            case entry::array:
               c.string( "array" );
               return;
            case entry::object:
               c.string( "object" );
               return;
            case entry::nothing:
               c.string( "nothing" );
               return;
            case entry::reference:
               c.string( "reference" );
               return;
         }
         assert( false );
      }
   };

   template<>
   struct format_traits< entry >
   {
      template< template< typename... > class Traits, typename Consumer >
      static void produce( Consumer& c, const entry& v )
      {
         c.begin_object();
         // c.key( "type" );
         // json::events::produce< Traits >( c, v.type() );
         // c.member();
         c.key( "clear" );
         c.boolean( v.clear() );
         c.member();
         switch( v.type() ) {
            case entry::atom:
               c.key( "atom" );
               json::events::produce< Traits >( c, v.get_atom() );
               break;
            case entry::array:
               c.key( "array" );
               json::events::produce< Traits >( c, v.get_array() );
               break;
            case entry::object:
               c.key( "object" );
               json::events::produce< Traits >( c, v.get_object() );
               break;
            case entry::nothing:
               assert( false );
            case entry::reference:
               c.key( "reference" );
               json::events::produce< Traits >( c, v.get_reference() );
               break;
            default:
               assert( false );
         }
         c.member();
         c.end_object();
      }
   };

   template<>
   struct format_traits< concat >
      : public json::binding::object< TAO_JSON_BIND_REQUIRED( "position", &concat::p ),
                                      TAO_JSON_BIND_REQUIRED( "concat_list", &concat::entries ) >
   {
   };

   template<>
   struct format_traits< entry_array >
      : public json::binding::object< TAO_JSON_BIND_REQUIRED( "position", &entry_array::position ),
                                      TAO_JSON_BIND_REQUIRED( "array_data", &entry_array::list ) >
   {
   };

   template<>
   struct format_traits< entry_object >
      : public json::binding::object< TAO_JSON_BIND_REQUIRED( "position", &entry_object::position ),
                                      TAO_JSON_BIND_REQUIRED( "object_data", &entry_object::map ) >
   {
   };

   template<>
   struct format_traits< const concat* >
   {
      template< template< typename... > class Traits >
      static void assign( json::basic_value< Traits >& v, const concat* p )
      {
         v.set_opaque_ptr( p );
      }
   };

   template<>
   struct format_traits< concat* >
      : format_traits< const concat* >
   {
   };

}  // namespace tao::config::internal

#endif
