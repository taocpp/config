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
            : public json::binding::object< TAO_JSON_BIND_REQUIRED( "source", &position::source ),
                                            TAO_JSON_BIND_REQUIRED( "line", &position::line ),
                                            TAO_JSON_BIND_REQUIRED( "byte_in_line", &position::byte_in_line ),
                                            TAO_JSON_BIND_REQUIRED( "byte", &position::byte ) >
         {
         };

         template<>
         struct traits< const pegtl::position* >
         {
            TAO_JSON_DEFAULT_KEY( "position" );

            template< template< typename... > class Traits >
            static void assign( json::basic_value< Traits >& v, const position* p )
            {
               v.unsafe_assign_opaque_ptr( p );
            }
         };

         template<>
         struct traits< pegtl::position* >
            : public traits< const position* >
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
