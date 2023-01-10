// Copyright (c) 2019-2023 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_INTERNAL_PHASE5_REPACK_HPP
#define TAO_CONFIG_INTERNAL_PHASE5_REPACK_HPP

#include <cassert>
#include <stdexcept>
#include <utility>

#include "array.hpp"
#include "concat.hpp"
#include "entry.hpp"
#include "forward.hpp"
#include "json.hpp"
#include "json_traits.hpp"
#include "object.hpp"
#include "repack_traits.hpp"

#include "../key.hpp"

namespace tao::config::internal
{
   template< template< typename... > class Traits >
   void set_key_and_position( [[maybe_unused]] json::basic_value< Traits >& r, [[maybe_unused]] const key& k, [[maybe_unused]] const pegtl::position& p )
   {
      if constexpr( has_set_key< json::basic_value< Traits > > ) {
         r.set_key( k );
      }
      if constexpr( has_set_position< json::basic_value< Traits > > ) {
         r.set_position( p );
      }
   }

   template< template< typename... > class Traits >
   void phase5_repack( const key& k, json::events::to_basic_value< Traits >& consumer, const concat& c );

   template< template< typename... > class Traits >
   void phase5_repack( const key& k, json::events::to_basic_value< Traits >& consumer, const array& a )
   {
      consumer.begin_array( a.array.size() );
      set_key_and_position( consumer.stack_.back(), k, a.position );
      std::size_t i = 0;
      for( const auto& c : a.array ) {
         phase5_repack( k + i++, consumer, c );
         consumer.element();
      }
      consumer.end_array( a.array.size() );
   }

   template< template< typename... > class Traits >
   void phase5_repack( const key& k, json::events::to_basic_value< Traits >& consumer, const object& o )
   {
      consumer.begin_object( o.object.size() );
      set_key_and_position( consumer.stack_.back(), k, o.position );
      for( const auto& p : o.object ) {
         consumer.key( p.first );
         phase5_repack( k + p.first, consumer, p.second );
         consumer.member();
      }
      consumer.end_object( o.object.size() );
   }

   template< template< typename... > class Traits >
   void phase5_repack( const key& k, json::events::to_basic_value< Traits >& consumer, const json_t& j )
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
            throw std::logic_error( "code should be unreachable" );  // LCOV_EXCL_LINE
      }
      set_key_and_position( consumer.value, k, j.position );
   }

   template< template< typename... > class Traits >
   void phase5_repack( const key& k, json::events::to_basic_value< Traits >& consumer, const entry& e )
   {
      switch( e.kind() ) {
         case entry_kind::value:
            phase5_repack( k, consumer, e.get_value() );
            return;
         case entry_kind::reference:
            throw std::logic_error( "code should be unreachable" );  // LCOV_EXCL_LINE -- must have been either eliminated or flagged as error earlier.
         case entry_kind::array:
            phase5_repack( k, consumer, e.get_array() );
            return;
         case entry_kind::object:
            phase5_repack( k, consumer, e.get_object() );
            return;
         case entry_kind::concat:
            throw std::logic_error( "code should be unreachable" );  // LCOV_EXCL_LINE -- must have been either eliminated or flagged as error earlier.
      }
      throw std::logic_error( "code should be unreachable" );  // LCOV_EXCL_LINE
   }

   template< template< typename... > class Traits >
   void phase5_repack( const key& k, json::events::to_basic_value< Traits >& consumer, const concat& c )
   {
      assert( c.concat.size() == 1 );  // This should be ensured by phase3_remove().

      phase5_repack( k, consumer, c.concat.front() );
   }

   template< template< typename... > class Traits >
   [[nodiscard]] json::basic_value< Traits > phase5_repack( const concat& c )
   {
      json::events::to_basic_value< Traits > consumer;
      phase5_repack( key(), consumer, c );
      return std::move( consumer.value );
   }

   template< template< typename... > class Traits >
   [[nodiscard]] json::basic_value< Traits > phase5_repack( const object& o )
   {
      json::events::to_basic_value< Traits > consumer;
      phase5_repack( key(), consumer, o );
      set_key_and_position( consumer.value, key(), o.position );
      return std::move( consumer.value );
   }

}  // namespace tao::config::internal

#endif
