// Copyright (c) 2018-2020 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_INTERNAL_PHASE1_APPEND_HPP
#define TAO_CONFIG_INTERNAL_PHASE1_APPEND_HPP

#include <cassert>
#include <iterator>
#include <optional>
#include <string>
#include <type_traits>

#include "array.hpp"
#include "concat.hpp"
#include "constants.hpp"
#include "entry.hpp"
#include "json.hpp"
#include "json_traits.hpp"
#include "key1.hpp"
#include "object.hpp"
#include "phase1_stuff.hpp"
#include "reference2.hpp"

namespace tao::config::internal
{
   inline bool phase1_append( concat& c, const json_t& value )
   {
      c.concat.emplace_back( value );
      return true;
   }

   inline bool phase1_append( concat& c, const reference2& value )
   {
      c.concat.emplace_back( value );
      return true;
   }

   inline bool phase1_append( concat& c, const phase1_stuff s )
   {
      switch( s ) {
         case phase1_stuff::remove_all:
            c.concat.clear();
            c.remove = true;
            c.temporary = false;
            return true;
         case phase1_stuff::ensure_array:
            c.back_ensure_kind( entry_kind::array, pegtl::position( 1, 1, 1, "TODO" ) );
            return true;
         case phase1_stuff::ensure_object:
            c.back_ensure_kind( entry_kind::object, pegtl::position( 1, 1, 1, "TODO" ) );
            return true;
         case phase1_stuff::make_permanent:
            c.temporary = false;
            return true;
         case phase1_stuff::make_temporary:
            c.temporary = true;
            return true;
      }
      assert( false );  // UNREACHABLE
   }

   inline bool phase1_append( concat& c, const std::optional< std::string >& s )
   {
      c.schema = s ? ( *s ) : std::string();
      return true;
   }

   template< typename V >
   bool phase1_append( concat& c, const key1& path, const V& value );

   template< typename V >
   bool phase1_append_star( concat& c, const key1& path, const V& value )
   {
      for( entry& e : c.concat ) {
         switch( e.kind() ) {
            case entry_kind::value:
               continue;
            case entry_kind::reference:
               continue;
            case entry_kind::array:
               for( concat& d : e.get_array().array ) {
                  phase1_append( d, path, value );
               }
               continue;
            case entry_kind::object:
               for( auto& p : e.get_object().object ) {
                  phase1_append( p.second, path, value );
               }
               continue;
         }
         assert( false );  // UNREACHABLE
      }
      return true;
   }

   template< typename V >
   bool phase1_append_name( concat& c, const pegtl::position& p, const std::string& name, const key1& path, const V& value )
   {
      c.back_ensure_kind( entry_kind::object, p );
      const auto pair = c.concat.back().get_object().object.try_emplace( name, p );
      pair.first->second.implicit = is_implicit_any( value ) && ( pair.second || pair.first->second.implicit );
      return phase1_append( pair.first->second, path, value );
   }

   template< typename V >
   bool phase1_append_index( concat& c, const pegtl::position& p, const std::size_t index, const key1& path, const V& value )
   {
      std::size_t n = index;

      for( auto& e : c.concat ) {
         switch( e.kind() ) {
            case entry_kind::value:
               throw std::string( "cannot index (across) value" );
            case entry_kind::reference:
               throw std::string( "cannot index (across) reference" );
            case entry_kind::array:
               if( e.get_array().array.size() > n ) {
                  return phase1_append( *std::next( e.get_array().array.begin(), n ), path, value );
               }
               n -= e.get_array().array.size();
               continue;
            case entry_kind::object:
               throw std::string( "cannot index (across) object" );
         }
      }
      throw std::string( "index out of range" );
   }

   template< typename V >
   bool phase1_append_append( concat& c, const pegtl::position& p, const std::uint64_t g, const key1& path, const V& value )
   {
      c.back_ensure_kind( entry_kind::array, p );
      auto& a = c.concat.back().get_array();
      if( g > c.generation ) {
         c.generation = g;
         return phase1_append( a.array.emplace_back( p ), path, value );
      }
      assert( !a.array.empty() );

      return phase1_append( a.array.back(), path, value );
   }

   template< typename V >
   bool phase1_append( concat& c, const key1& path, const V& value )
   {
      if( path.empty() ) {
         return phase1_append( c, value );
      }
      const auto& part = path.at( 0 );

      switch( part.kind() ) {
         case key1_kind::star:
            return phase1_append_star( c, pop_front( path ), value );
         case key1_kind::name:
            return phase1_append_name( c, part.position, part.get_name(), pop_front( path ), value );
         case key1_kind::index:
            return phase1_append_index( c, part.position, part.get_index(), pop_front( path ), value );
         case key1_kind::append:
            return phase1_append_append( c, part.position, part.get_generation(), pop_front( path ), value );
      }
      assert( false );  // UNREACHABLE
   }

   template< typename V >
   void phase1_append( object& o, const key1& path, const V& value )
   {
      assert( !path.empty() );

      const std::string& name = path.front().get_name();  // TODO: Error message if other type.
      const auto pair = o.object.try_emplace( name, path.front().position );
      pair.first->second.implicit = is_implicit_any( value ) && ( pair.second || pair.first->second.implicit );
      phase1_append( pair.first->second, pop_front( path ), value );
   }

}  // namespace tao::config::internal

#endif
