// Copyright (c) 2018 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_INTERNAL_TO_STREAM_HPP
#define TAO_CONFIG_INTERNAL_TO_STREAM_HPP

#include <ostream>

#include "json.hpp"
#include "value.hpp"

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
         struct traits< json::value >
         {
            template< template< typename... > class, typename Consumer >
            static void produce( Consumer& c, const json::value& v )
            {
               json::events::from_value( c, v );
            }
         };

         template<>
         struct traits< entry >
         {
            template< template< typename... > class, typename Consumer >
            static void produce( Consumer& c, const entry& v )
            {
               switch( v.type() ) {
                  case type::ATOM:
                     json::events::produce< traits >( c, v.get_atom() );
                     return;
                  case type::ARRAY:
                     json::events::produce< traits >( c, v.get_array() );
                     return;
                  case type::OBJECT:
                     json::events::produce< traits >( c, v.get_object() );
                     return;
                  case type::NOTHING:
                     assert( false );
                  case type::INDIRECT:
                     json::events::produce< traits >( c, v.get_indirect() );
                     return;
               }
               assert( false );
            }
         };

         template< typename T >
         inline void to_stream( std::ostream& os, const T& t )
         {
            json::jaxn::events::to_stream consumer( os );
            json::events::produce< traits >( consumer, t );
         }

         template< typename T >
         inline void to_stream( std::ostream& os, const T& t, const std::size_t indent )
         {
            json::jaxn::events::to_pretty_stream consumer( os, indent );
            json::events::produce< traits >( consumer, t );
         }

      }  // namespace internal

   }  // namespace config

}  // namespace tao

#endif
