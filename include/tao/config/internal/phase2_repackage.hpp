// Copyright (c) 2019-2020 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_INTERNAL_PHASE2_REPACKAGE_HPP
#define TAO_CONFIG_INTERNAL_PHASE2_REPACKAGE_HPP

#include <utility>

#include "../key.hpp"

#include "json.hpp"
#include "key_utility.hpp"
#include "type_traits.hpp"
#include "value_traits.hpp"

namespace tao::config::internal
{
   template< template< typename... > class Traits >
   void phase2_set_keys( json::basic_value< Traits >& r, const config::key& k )
   {
      switch( r.type() ) {
         case json::type::ARRAY:
            for( std::size_t i = 0; i < r.get_array().size(); ++i ) {
               phase2_set_keys( r[ i ], k + i );
            }
            break;
         case json::type::OBJECT:
            for( auto& i : r.get_object() ) {
               phase2_set_keys( i.second, k + i.first );
            }
            break;
         default:
            break;
      }
      r.set_key( k );
   }

   template< template< typename... > class Traits >
   void phase2_set_positions( json::basic_value< Traits >& r, const json::basic_value< value_traits >& v )
   {
      switch( r.type() ) {
         case json::type::ARRAY:
            for( std::size_t i = 0; i < r.get_array().size(); ++i ) {
               phase2_set_positions( r[ i ], v.at( i ) );
            }
            break;
         case json::type::OBJECT:
            for( auto& i : r.get_object() ) {
               phase2_set_positions( i.second, v.at( i.first ) );
            }
            break;
         default:
            break;
      }
      if( v.position ) {
         r.set_position( *v.position );  // TODO: Find out when it's missing and why.
      }
   }

   inline void phase2_filter_temporaries( const std::vector< key >& temporaries, json::basic_value< value_traits >& v )
   {
      for( const key& k : temporaries ) {
         try {
            v.erase( key_to_pointer( k ) );
         }
         catch( ... ) {
         }
      }
   }

   template< template< typename... > class Traits >
   json::basic_value< Traits > phase2_repackage( const json::basic_value< value_traits >& v )
   {
      json::events::to_basic_value< Traits > consumer;
      json::events::from_value( consumer, v );

      if constexpr( has_set_key< json::basic_value< Traits > > ) {
         phase2_set_keys( consumer.value, config::key() );
      }
      if constexpr( has_set_position< json::basic_value< Traits > > ) {
         phase2_set_positions( consumer.value, v );
         consumer.value.set_position( json::position( "(root)", 1, 0 ) );
      }
      return std::move( consumer.value );
   }

}  // namespace tao::config::internal

#endif
