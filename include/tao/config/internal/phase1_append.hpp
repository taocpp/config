// Copyright (c) 2018-2024 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_INTERNAL_PHASE1_APPEND_HPP
#define TAO_CONFIG_INTERNAL_PHASE1_APPEND_HPP

#include <cassert>
#include <cstddef>
#include <iterator>
#include <optional>
#include <set>
#include <stdexcept>
#include <string>
#include <type_traits>

#include "array.hpp"
#include "concat.hpp"
#include "constants.hpp"
#include "entry.hpp"
#include "json.hpp"
#include "key1.hpp"
#include "limits.hpp"
#include "object.hpp"
#include "phase1_mode.hpp"
#include "reference2.hpp"

namespace tao::config::internal
{
   template< typename T >
   void phase1_append( concat& c, const key1& path, const T& thing, const phase1_mode mode );

   template< typename T >
   void phase1_append_asterisk( concat& c, const pegtl::position& p, const key1& path, const T& thing, const phase1_mode mode )
   {
      c.back_ensure_init( asterisk_init, p );
      phase1_append( c.concat.back().get_asterisk(), path, thing, mode );
   }

   template< typename T >
   void phase1_append_name( concat& c, const pegtl::position& p, const std::string& name, const key1& path, const T& thing, const phase1_mode mode )
   {
      c.back_ensure_init( object_init, p );
      const auto pair = c.concat.back().get_object().object.try_emplace( name, p );
      pair.first->second.implicit = ( mode == phase1_mode::implicit ) && ( pair.second || pair.first->second.implicit );
      phase1_append( pair.first->second, path, thing, mode );
   }

   template< typename T >
   void phase1_append_index( concat& c, const pegtl::position& p, const std::size_t index, const key1& path, const T& thing, const phase1_mode mode )
   {
      std::size_t n = index;

      for( auto& e : c.concat ) {
         switch( e.kind() ) {
            case entry_kind::NULL_:
            case entry_kind::BOOLEAN:
            case entry_kind::STRING:
            case entry_kind::BINARY:
            case entry_kind::SIGNED:
            case entry_kind::UNSIGNED:
            case entry_kind::DOUBLE:
               throw pegtl::parse_error( "cannot index (across) value", p );
            case entry_kind::ARRAY:
               if( e.get_array().array.size() > n ) {
                  phase1_append( *std::next( e.get_array().array.begin(), n ), path, thing, mode );
                  return;
               }
               n -= e.get_array().array.size();
               continue;
            case entry_kind::OBJECT:
               throw pegtl::parse_error( "cannot index (across) object", p );
            case entry_kind::ASTERISK:
               throw pegtl::parse_error( "cannot index (across) asterisk", p );
            case entry_kind::REFERENCE:
               throw pegtl::parse_error( "cannot index (across) reference", p );
         }
      }
      throw pegtl::parse_error( "index out of range", p );
   }

   template< typename T >
   void phase1_append_append( concat& c, const pegtl::position& p, const std::uint64_t g, const key1& path, const T& thing, const phase1_mode mode )
   {
      c.back_ensure_init( array_init, p );
      auto& a = c.concat.back().get_array();
      if( g > c.generation ) {
         c.generation = g;
         concat& d = a.array.emplace_back( p );
         d.remove = false;  // TODO: Make consistent with entry::expand()?
         phase1_append( d, path, thing, mode );
         return;
      }
      assert( !a.array.empty() );
      phase1_append( a.array.back(), path, thing, mode );
   }

   template< typename T >
   void phase1_append( concat& c, const key1& path, const T& thing, const phase1_mode mode )
   {
      if( path.empty() ) {
         thing( c );
         return;
      }
      const auto& part = path.at( 0 );

      switch( part.kind() ) {
         case key1_kind::asterisk:
            phase1_append_asterisk( c, part.position, pop_front( path ), thing, mode );
            return;
         case key1_kind::name:
            phase1_append_name( c, part.position, part.get_name(), pop_front( path ), thing, mode );
            return;
         case key1_kind::index:
            phase1_append_index( c, part.position, part.get_index(), pop_front( path ), thing, mode );
            return;
         case key1_kind::append:
            phase1_append_append( c, part.position, part.get_generation(), pop_front( path ), thing, mode );
            return;
      }
      throw std::logic_error( "code should be unreachable" );  // LCOV_EXCL_LINE
   }

   template< typename T >
   void phase1_append( object& o, const key1& path, const T& thing, const phase1_mode mode )
   {
      assert( !path.empty() );

      if( path.size() > global_nesting_limit ) {
         throw pegtl::parse_error( "nesting depth exceeded", path.at( global_nesting_limit ).position );
      }
      if( path.front().kind() != key1_kind::name ) {
         throw pegtl::parse_error( "expected name", path.front().position );
      }
      const std::string& name = path.front().get_name();
      const auto pair = o.object.try_emplace( name, path.front().position );
      pair.first->second.implicit = ( mode == phase1_mode::implicit ) && ( pair.second || pair.first->second.implicit );
      phase1_append( pair.first->second, pop_front( path ), thing, mode );
   }

}  // namespace tao::config::internal

#endif
