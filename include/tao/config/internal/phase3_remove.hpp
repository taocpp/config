// Copyright (c) 2019-2023 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_INTERNAL_PHASE3_REMOVE_HPP
#define TAO_CONFIG_INTERNAL_PHASE3_REMOVE_HPP

#include <stdexcept>

#include "array.hpp"
#include "concat.hpp"
#include "entry.hpp"
#include "forward.hpp"
#include "object.hpp"
#include "pegtl.hpp"
#include "string_utility.hpp"

namespace tao::config::internal
{
   inline void phase3_remove( array& a );
   inline void phase3_remove( object& o );

   [[nodiscard]] inline std::string phase3_partial_type( const entry& e )
   {
      switch( e.kind() ) {
         case entry_kind::value:
            return std::string( json::to_string( e.get_value().type() ) );
         case entry_kind::reference:
            throw std::logic_error( "code should be unreachable" );  // LCOV_EXCL_LINE
         case entry_kind::array:
            return "array";
         case entry_kind::object:
            return "object";
         case entry_kind::concat:
            throw std::logic_error( "code should be unreachable" );  // LCOV_EXCL_LINE
      }
      throw std::logic_error( "code should be unreachable" );  // LCOV_EXCL_LINE
   }

   [[nodiscard]] inline pegtl::position phase3_partial_position( const entry& e )
   {
      switch( e.kind() ) {
         case entry_kind::value:
            return e.get_value().position;
         case entry_kind::reference:
            throw std::logic_error( "code should be unreachable" );  // LCOV_EXCL_LINE
         case entry_kind::array:
            return e.get_array().position;
         case entry_kind::object:
            return e.get_object().position;
         case entry_kind::concat:
            throw std::logic_error( "code should be unreachable" );  // LCOV_EXCL_LINE
      }
      throw std::logic_error( "code should be unreachable" );  // LCOV_EXCL_LINE
   }

   inline void phase3_remove( concat& c )
   {
      for( auto& e : c.concat ) {
         switch( e.kind() ) {
            case entry_kind::value:
               continue;
            case entry_kind::reference:
               throw pegtl::parse_error( "unresolved reference '" + e.get_reference().to_string() + '\'', e.get_reference().at( 0 ).position );
            case entry_kind::array:
               phase3_remove( e.get_array() );
               continue;
            case entry_kind::object:
               phase3_remove( e.get_object() );
               continue;
            case entry_kind::concat:
               throw pegtl::parse_error( "unresolved star", e.get_concat().position );  // Can happen when there are also unresolved references.
         }
         throw std::logic_error( "code should be unreachable" );  // LCOV_EXCL_LINE
      }
      if( c.concat.size() > 1 ) {
         auto i = c.concat.begin();
         const auto lt = phase3_partial_type( *i );
         const auto lp = phase3_partial_position( *i );
         ++i;
         const auto rt = phase3_partial_type( *i );
         const auto rp = phase3_partial_position( *i );
         throw pegtl::parse_error( strcat( "incompatible types ", lt, "@", lp, " and ", rt, "@", rp ), c.position );
      }
   }

   inline void phase3_remove( array& a )
   {
      auto i = a.array.begin();

      while( i != a.array.end() ) {
         phase3_remove( *i );
         if( i->omit_from_final_result() ) {
            i = a.array.erase( i );
         }
         else {
            ++i;
         }
      }
   }

   inline void phase3_remove( object& o )
   {
      auto i = o.object.begin();

      while( i != o.object.end() ) {
         phase3_remove( i->second );
         if( i->second.omit_from_final_result() ) {
            i = o.object.erase( i );
         }
         else {
            ++i;
         }
      }
   }

}  // namespace tao::config::internal

#endif
