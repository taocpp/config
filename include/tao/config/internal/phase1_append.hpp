// Copyright (c) 2018-2021 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_INTERNAL_PHASE1_APPEND_HPP
#define TAO_CONFIG_INTERNAL_PHASE1_APPEND_HPP

#include <cassert>
#include <iterator>
#include <optional>
#include <set>
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
#include "reverse.hpp"

namespace tao::config::internal
{
   template< typename T >
   bool phase1_append( concat& c, const key1& path, const T& thing, const bool implicit );

   // template< typename T >
   // bool phase1_append_star( concat& c, const pegtl::position&, const key1& path, const T& thing, const bool implicit )
   // {
   //    std::set< std::string > done;

   //    for( entry& e : reverse( c.concat ) ) {
   //       switch( e.kind() ) {
   //          case entry_kind::value:
   //             continue;
   //          case entry_kind::reference:
   //             continue;
   //          case entry_kind::array:
   //             for( concat& d : e.get_array().array ) {
   //                phase1_append( d, path, thing, implicit );
   //             }
   //             continue;
   //          case entry_kind::object:
   //             for( auto& p : e.get_object().object ) {
   //                if( done.emplace( p.first ).second ) {
   //                   phase1_append( p.second, path, thing, implicit );
   //                }
   //             }
   //             continue;
   //          case entry_kind::concat:
   //             assert( false );  // UNREACHABLE
   //       }
   //       assert( false );  // UNREACHABLE
   //    }
   //    return true;
   // }

   template< typename T >
   bool phase1_append_star( concat& c, const pegtl::position& p, const key1& path, const T& thing, const bool implicit )
   {
      c.back_ensure_kind( entry_kind::concat, p );
      return phase1_append( c.concat.back().get_concat(), path, thing, implicit );
   }

   template< typename T >
   bool phase1_append_name( concat& c, const pegtl::position& p, const std::string& name, const key1& path, const T& thing, const bool implicit )
   {
      c.back_ensure_kind( entry_kind::object, p );
      const auto pair = c.concat.back().get_object().object.try_emplace( name, p );
      pair.first->second.implicit = implicit && ( pair.second || pair.first->second.implicit );
      return phase1_append( pair.first->second, path, thing, implicit );
   }

   template< typename T >
   bool phase1_append_index( concat& c, const pegtl::position& p, const std::size_t index, const key1& path, const T& thing, const bool implicit )
   {
      std::size_t n = index;

      for( auto& e : c.concat ) {
         switch( e.kind() ) {
            case entry_kind::value:
               throw pegtl::parse_error( "cannot index (across) value", p );
            case entry_kind::reference:
               throw pegtl::parse_error( "cannot index (across) reference", p );
            case entry_kind::array:
               if( e.get_array().array.size() > n ) {
                  return phase1_append( *std::next( e.get_array().array.begin(), n ), path, thing, implicit );
               }
               n -= e.get_array().array.size();
               continue;
            case entry_kind::object:
               throw pegtl::parse_error( "cannot index (across) object", p );
            case entry_kind::concat:
               throw pegtl::parse_error( "cannot index (across) star", p );
         }
      }
      throw std::string( "index out of range" );
   }

   template< typename T >
   bool phase1_append_append( concat& c, const pegtl::position& p, const std::uint64_t g, const key1& path, const T& thing, const bool implicit )
   {
      c.back_ensure_kind( entry_kind::array, p );
      auto& a = c.concat.back().get_array();
      if( g > c.generation ) {
         c.generation = g;
         return phase1_append( a.array.emplace_back( p ), path, thing, implicit );
      }
      assert( !a.array.empty() );

      return phase1_append( a.array.back(), path, thing, implicit );
   }

   template< typename T >
   bool phase1_append( concat& c, const key1& path, const T& thing, const bool implicit )
   {
      if( path.empty() ) {
         thing( c );
         return true;
      }
      const auto& part = path.at( 0 );

      switch( part.kind() ) {
         case key1_kind::star:
            return phase1_append_star( c, part.position, pop_front( path ), thing, implicit );
         case key1_kind::name:
            return phase1_append_name( c, part.position, part.get_name(), pop_front( path ), thing, implicit );
         case key1_kind::index:
            return phase1_append_index( c, part.position, part.get_index(), pop_front( path ), thing, implicit );
         case key1_kind::append:
            return phase1_append_append( c, part.position, part.get_generation(), pop_front( path ), thing, implicit );
      }
      assert( false );  // UNREACHABLE
   }

   template< typename T >
   void phase1_append( object& o, const key1& path, const T& thing, const bool implicit )
   {
      assert( !path.empty() );

      const std::string& name = path.front().get_name();  // TODO: Error message if other type.
      const auto pair = o.object.try_emplace( name, path.front().position );
      pair.first->second.implicit = implicit && ( pair.second || pair.first->second.implicit );
      phase1_append( pair.first->second, pop_front( path ), thing, implicit );
   }

}  // namespace tao::config::internal

#endif
