// Copyright (c) 2019-2020 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_INTERNAL_PHASE3_REPACK_HPP
#define TAO_CONFIG_INTERNAL_PHASE3_REPACK_HPP

#include <utility>

#include "array.hpp"
#include "concat.hpp"
#include "entry.hpp"
#include "forward.hpp"
#include "json.hpp"
#include "json_traits.hpp"
#include "object.hpp"
#include "phase3_traits.hpp"

#include "../key.hpp"

namespace tao::config::internal
{
   template< template< typename... > class Traits >
   void set_key_and_position( json::basic_value< Traits >& r, const key& k, const pegtl::position& p )
   {
      if constexpr( has_set_key< json::basic_value< Traits > > ) {
         r.set_key( k );
      }
      if constexpr( has_set_position< json::basic_value< Traits > > ) {
         r.set_position( p );
      }
   }

   template< template< typename... > class Traits >
   void phase3_repack( const key& k, json::events::to_basic_value< Traits >& consumer, const concat& c );

   template< template< typename... > class Traits >
   void phase3_repack( const key& k, json::events::to_basic_value< Traits >& consumer, const array& a )
   {
      consumer.begin_array( a.array.size() );
      set_key_and_position( consumer.stack_.back(), k, a.position );
      std::size_t i = 0;
      for( const auto& c : a.array ) {
         phase3_repack( k + i++, consumer, c );
         consumer.element();
      }
      consumer.end_array( a.array.size() );
   }

   template< template< typename... > class Traits >
   void phase3_repack( const key& k, json::events::to_basic_value< Traits >& consumer, const object& o )
   {
      consumer.begin_object( o.object.size() );
      set_key_and_position( consumer.stack_.back(), k, o.position );
      for( const auto& p : o.object ) {
         consumer.key( p.first );
         phase3_repack( k + p.first, consumer, p.second );
         consumer.member();
      }
      consumer.end_object( o.object.size() );
   }

   template< template< typename... > class Traits >
   void phase3_repack( const key& k, json::events::to_basic_value< Traits >& consumer, const json_t& j )
   {
      switch( j.type() ) {
         case json::type::NULL_:
            consumer.null();
            break;
         case json::type::BOOLEAN:
            consumer.boolean( j.get_boolean() );
            break;
         case json::type::SIGNED:
            consumer.number( j.get_signed() );
            break;
         case json::type::UNSIGNED:
            consumer.number( j.get_unsigned() );
            break;
         case json::type::DOUBLE:
            consumer.number( j.get_double() );
            break;
         case json::type::STRING:
            consumer.string( j.get_string() );
            break;
         case json::type::BINARY:
            consumer.binary( j.get_binary() );
            break;
         default:
            assert( false );  // UNREACHABLE
      }
      set_key_and_position( consumer.value, k, j.position );
   }

   template< template< typename... > class Traits >
   void phase3_repack( const key& k, json::events::to_basic_value< Traits >& consumer, const entry& e )
   {
      switch( e.kind() ) {
         case entry_kind::value:
            phase3_repack( k, consumer, e.get_value() );
            return;
         case entry_kind::reference:
            assert( false );  // UNREACHABLE
         case entry_kind::array:
            phase3_repack( k, consumer, e.get_array() );
            return;
         case entry_kind::object:
            phase3_repack( k, consumer, e.get_object() );
            return;
         case entry_kind::remove:
            assert( false );  // UNREACHABLE
      }
      assert( false );  // UNREACHABLE
   }

   template< template< typename... > class Traits >
   void phase3_repack( const key& k, json::events::to_basic_value< Traits >& consumer, const concat& c )
   {
      assert( c.concat.size() == 1 );  // This should be ensured by phase2_remove().

      phase3_repack( k, consumer, c.concat.front() );
   }

   template< template< typename... > class Traits >
   [[nodiscard]] json::basic_value< Traits > phase3_repack( const object& o )
   {
      json::events::to_basic_value< Traits > consumer;
      phase3_repack( key(), consumer, o );
      set_key_and_position( consumer.value, key(), o.position );
      return std::move( consumer.value );
   }

}  // namespace tao::config::internal

#endif
