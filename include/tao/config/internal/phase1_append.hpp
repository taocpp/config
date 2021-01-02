// Copyright (c) 2018-2020 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_INTERNAL_PHASE1_APPEND_HPP
#define TAO_CONFIG_INTERNAL_PHASE1_APPEND_HPP

#include <cassert>
#include <string>

#include "array.hpp"
#include "concat.hpp"
#include "constants.hpp"
#include "entry.hpp"
#include "json.hpp"
#include "json_traits.hpp"
#include "key1.hpp"
#include "object.hpp"
#include "reference2.hpp"

namespace tao::config::internal
{
   inline bool phase1_append( concat& c, const temporary_t )
   {
      c.temporary = true;
      return true;
   }

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

   inline bool phase1_append( concat& c, const entry_kind k )
   {
      if( k == entry_kind::remove ) {
         c.concat.clear();
         c.temporary = false;
      }
      c.back_ensure_kind( k, pegtl::position( 1, 1, 1, "TODO" ) );
      return true;
   }

   template< typename V >
   bool phase1_append( concat& c, const key1& path, const V& value );

   template< typename V >
   bool phase1_append_star( concat& c, const key1& path, const V& value )
   {
      for( auto& e : reverse( c.concat ) ) {
         switch( e.kind() ) {
            case entry_kind::value:
               continue;  // TODO: Error or ignore? -- Possibly ignore!
            case entry_kind::reference:
               continue;  // TODO: Error or ignore? -- Possibly ignore!
            case entry_kind::array:
               for( auto& c : e.get_array().array ) {
                  phase1_append( c, path, value );
               }
               continue;
            case entry_kind::object:
               for( auto& c : e.get_object().object ) {
                  phase1_append( c.second, path, value );
               }
               continue;
            case entry_kind::remove:
               return true;
         }
         assert( false );  // UNREACHABLE
      }
      return true;
   }

   template< typename V >
   bool phase1_append_minus( concat& c, const pegtl::position& p, const key1& path, const V& value )
   {
      c.back_ensure_kind( entry_kind::array, p );
      throw std::string( "TODO: " ) + __FUNCTION__;
   }

   template< typename V >
   bool phase1_append_name( concat& c, const pegtl::position& p, const std::string& name, const key1& path, const V& value )
   {
      c.back_ensure_kind( entry_kind::object, p );
      const auto pair = c.concat.back().get_object().object.try_emplace( name, p );
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
                  auto i = e.get_array().array.begin();
                  std::advance( i, n );
                  return phase1_append( *i, path, value );
               }
               n -= e.get_array().array.size();
               continue;
            case entry_kind::object:
               throw std::string( "cannot index (across) object" );
            case entry_kind::remove:
               n = index;
               continue;  // TODO: Skip to after the remove before iterating...
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
         case key1_kind::minus:
            return phase1_append_minus( c, part.position, pop_front( path ), value );
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
      phase1_append( pair.first->second, pop_front( path ), value );
   }

}  // namespace tao::config::internal

#endif
