// Copyright (c) 2018 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_INTERNAL_TRAITS_HPP
#define TAO_CONFIG_INTERNAL_TRAITS_HPP

#include <cassert>

#include "json.hpp"
#include "token.hpp"
#include "entry.hpp"

namespace tao
{
   namespace config
   {
      namespace internal
      {
         template< typename T >
         struct traits
            : public json::traits< T >
         {
         };

         template<>
         struct traits< token >
         {
            template< template< typename... > class, typename Consumer >
            static void produce( Consumer& c, const token& t )
            {
               switch( t.type() ) {
                  case token::NAME:
                     c.string( t.name() );
                     return;
                  case token::INDEX:
                     c.number( t.index() );
                     return;
                  case token::STAR:
                     c.string( "*" );
                     return;
                  case token::MINUS:
                     c.string( "-" );
                     return;
               }
               assert( false );
            }
         };

         template<>
         struct traits< entry >
         {
            template< template< typename... > class, typename Consumer >
            static void produce( Consumer& c, const entry& v )
            {
               switch( v.type() ) {
                  case entry::ATOM:
                     json::events::produce< traits >( c, v.get_atom() );
                     return;
                  case entry::ARRAY:
                     json::events::produce< traits >( c, v.get_array() );
                     return;
                  case entry::OBJECT:
                     json::events::produce< traits >( c, v.get_object() );
                     return;
                  case entry::NOTHING:
                     assert( false );
                  case entry::INDIRECT:
                     json::events::produce< traits >( c, v.get_indirect() );
                     return;
               }
               assert( false );
            }
         };

         template<>
         struct traits< pegtl::position >
            : public json::binding::object< TAO_JSON_BIND_REQUIRED( "source", &pegtl::position::source ),
                                            TAO_JSON_BIND_REQUIRED( "line", &pegtl::position::line ),
                                            TAO_JSON_BIND_REQUIRED( "byte_in_line", &pegtl::position::byte_in_line ),
                                            TAO_JSON_BIND_REQUIRED( "byte", &pegtl::position::byte ) >
         {
         };

         template<>
         struct traits< concat >
            : public json::binding::object< TAO_JSON_BIND_REQUIRED( "position", &concat::p ),
                                            TAO_JSON_BIND_REQUIRED( "concat", &concat::v ) >
         {
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

      }  // namespace internal

   }  // namespace config

}  // namespace tao

#endif
