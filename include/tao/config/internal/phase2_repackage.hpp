// Copyright (c) 2019 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_INTERNAL_PHASE2_REPACKAGE_HPP
#define TAO_CONFIG_INTERNAL_PHASE2_REPACKAGE_HPP

#include <utility>

#include "../key.hpp"

#include "json.hpp"
#include "phase2_traits.hpp"
#include "type_traits.hpp"

namespace tao::config::internal
{
   template< template< typename... > class Traits >
   void phase2_set_keys( json::basic_value< Traits >& r, const config::key& k )
   {
      switch( r.type() ) {
         case json::type::ARRAY:
            for( std::size_t i = 0; i < r.unsafe_get_array().size(); ++i ) {
               phase2_set_keys( r[ i ], k + i );
            }
            break;
         case json::type::OBJECT:
            for( auto& i : r.unsafe_get_object() ) {
               phase2_set_keys( i.second, k + i.first );
            }
            break;
         default:
            break;
      }
      r.set_key( k );
   }

   template< template< typename... > class Traits >
   void phase2_set_positions( json::basic_value< Traits >& r, const json::basic_value< phase2_traits >& v )
   {
      switch( r.type() ) {
         case json::type::ARRAY:
            for( std::size_t i = 0; i < r.unsafe_get_array().size(); ++i ) {
               phase2_set_positions( r[ i ], v.at( i ) );
            }
            break;
         case json::type::OBJECT:
            for( auto& i : r.unsafe_get_object() ) {
               phase2_set_positions( i.second, v.at( i.first ) );
            }
            break;
         default:
            break;
      }
      r.set_position( v.position );
   }

   template< template< typename... > class Traits >
   json::basic_value< Traits > phase2_repackage( const std::string& source, const json::basic_value< phase2_traits >& v )
   {
      json::events::to_basic_value< Traits > consumer;
      json::events::from_value( consumer, v );

      if constexpr( has_set_key< json::basic_value< Traits > >::value ) {
         phase2_set_keys( consumer.value, config::key() );
      }
      if constexpr( has_set_position< json::basic_value< Traits > >::value ) {
         phase2_set_positions( consumer.value, v );
         consumer.value.set_position( json::position( source, 0, 0 ) );
      }
      return std::move( consumer.value );
   }

}  // namespace tao::config::internal

#endif
