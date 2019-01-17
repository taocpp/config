// Copyright (c) 2018-2019 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_INTERNAL_TRAITS_HPP
#define TAO_CONFIG_INTERNAL_TRAITS_HPP

#include "concat.hpp"
#include "entry.hpp"
#include "json.hpp"

namespace tao
{
   namespace config
   {
      namespace internal
      {
         template< typename T >
         struct traits
            : public config::traits< T >
         {
         };

         template<>
         struct traits< void >
            : public json::traits< void >
         {
         };

         template<>
         struct traits< pegtl::position >
         {
            template< template< typename... > class Traits, typename Consumer >
            static void produce( Consumer& c, const pegtl::position& p )
            {
               c.string( p.source + ':' + std::to_string( p.line ) + ':' + std::to_string( p.byte_in_line ) );
            }
         };

         template<>
         struct traits< const pegtl::position* >
         {
            TAO_JSON_DEFAULT_KEY( "position" );

            template< template< typename... > class Traits >
            static void assign( json::basic_value< Traits >& v, const pegtl::position* p )
            {
               v.unsafe_assign_opaque_ptr( p );
            }
         };

         template<>
         struct traits< pegtl::position* >
            : public traits< const pegtl::position* >
         {
         };

         template<>
         struct traits< json::type >
         {
            TAO_JSON_DEFAULT_KEY( "type" );

            template< template< typename... > class Traits >
            static void assign( json::basic_value< Traits >& v, const json::type t )
            {
               v.unsafe_assign_string( std::string( json::to_string( t ) ) );
            }
         };

         template<>
         struct traits< json::value >
         {
            template< template< typename... > class, typename Consumer >
            static void produce( Consumer& c, const json::value& v )
            {
               json::events::from_value( c, v );
            }
         };

         template<>
         struct traits< entry::kind >
         {
            TAO_JSON_DEFAULT_KEY( "type" );

            template< template< typename... > class Traits >
            static void assign( json::basic_value< Traits >& v, const entry::kind k )
            {
               switch( k ) {
                  case entry::atom:
                     v.unsafe_assign_string( "atom" );
                     return;
                  case entry::array:
                     v.unsafe_assign_string( "array" );
                     return;
                  case entry::object:
                     v.unsafe_assign_string( "object" );
                     return;
                  case entry::nothing:
                     v.unsafe_assign_string( "nothing" );
                     return;
                  case entry::reference:
                     v.unsafe_assign_string( "reference" );
                     return;
               }
               assert( false );
            }
         };

         template<>
         struct traits< entry >
         {
            template< template< typename... > class Traits, typename Consumer >
            static void produce( Consumer& c, const entry& v )
            {
               switch( v.type() ) {
                  case entry::atom:
                     json::events::produce< Traits >( c, v.get_atom() );
                     return;
                  case entry::array:
                     json::events::produce< Traits >( c, v.get_array() );
                     return;
                  case entry::object:
                     json::events::produce< Traits >( c, v.get_object() );
                     return;
                  case entry::nothing:
                     assert( false );
                  case entry::reference:
                     json::events::produce< Traits >( c, v.get_reference() );
                     return;
               }
               assert( false );
            }
         };

         template<>
         struct traits< concat >
            : public json::binding::object< TAO_JSON_BIND_REQUIRED( "position", &concat::p ),
                                            TAO_JSON_BIND_REQUIRED( "concat", &concat::entries ),
                                            TAO_JSON_BIND_REQUIRED( "temporary", &concat::is_temporary ) >
         {
         };

      }  // namespace internal

   }  // namespace config

}  // namespace tao

#endif
