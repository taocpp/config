// Copyright (c) 2018 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_INTERNAL_TRAITS_HPP
#define TAO_CONFIG_INTERNAL_TRAITS_HPP

#include "json.hpp"
#include "pegtl.hpp"

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
         struct traits< position >
         {
            template< template< typename... > class Traits, typename Consumer >
            static void produce( Consumer& c, const position& p )
            {
               c.string( p.source + ':' + std::to_string( p.line ) + ':' + std::to_string( p.byte_in_line ) );
            }
         };

         template<>
         struct traits< const position* >
         {
            TAO_JSON_DEFAULT_KEY( "position" );

            template< template< typename... > class Traits >
            static void assign( json::basic_value< Traits >& v, const position* p )
            {
               v.unsafe_assign_opaque_ptr( p );
            }
         };

         template<>
         struct traits< position* >
            : public traits< const position* >
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

      }  // namespace internal

   }  // namespace config

}  // namespace tao

#endif
