// Copyright (c) 2019-2024 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_INTERNAL_PHASE3_REMOVE_HPP
#define TAO_CONFIG_INTERNAL_PHASE3_REMOVE_HPP

#include <stdexcept>
#include <string>

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
         case entry_kind::NULL_:
            return "null";
         case entry_kind::BOOLEAN:
            return "boolean";
         case entry_kind::STRING:
            return "string";
         case entry_kind::BINARY:
            return "binary";
         case entry_kind::SIGNED:
            return "signed";
         case entry_kind::UNSIGNED:
            return "unsigned";
         case entry_kind::DOUBLE:
            return "double";
         case entry_kind::ARRAY:
            return "array";
         case entry_kind::OBJECT:
            return "object";
         case entry_kind::ASTERISK:
            throw std::logic_error( "code should be unreachable" );  // LCOV_EXCL_LINE
         case entry_kind::REFERENCE:
            throw std::logic_error( "code should be unreachable" );  // LCOV_EXCL_LINE
      }
      throw std::logic_error( "code should be unreachable" );  // LCOV_EXCL_LINE
   }

   [[nodiscard]] inline pegtl::position phase3_partial_position( const entry& e )
   {
      switch( e.kind() ) {
         case entry_kind::NULL_:
         case entry_kind::BOOLEAN:
         case entry_kind::STRING:
         case entry_kind::BINARY:
         case entry_kind::SIGNED:
         case entry_kind::UNSIGNED:
         case entry_kind::DOUBLE:
         case entry_kind::ARRAY:
         case entry_kind::OBJECT:
            return e.get_position();
         case entry_kind::ASTERISK:
            throw std::logic_error( "code should be unreachable" );  // LCOV_EXCL_LINE
         case entry_kind::REFERENCE:
            throw std::logic_error( "code should be unreachable" );  // LCOV_EXCL_LINE
      }
      throw std::logic_error( "code should be unreachable" );  // LCOV_EXCL_LINE
   }

   inline void phase3_remove( concat& c )
   {
      for( auto& e : c.concat ) {
         switch( e.kind() ) {
            case entry_kind::NULL_:
            case entry_kind::BOOLEAN:
            case entry_kind::STRING:
            case entry_kind::BINARY:
            case entry_kind::SIGNED:
            case entry_kind::UNSIGNED:
            case entry_kind::DOUBLE:
               continue;
            case entry_kind::ARRAY:
               if( !e.get_array().function.empty() ) {
                  throw pegtl::parse_error( "function '" + e.get_array().function + "' could not be called", e.get_array().position );
               }
               phase3_remove( e.get_array() );
               continue;
            case entry_kind::OBJECT:
               phase3_remove( e.get_object() );
               continue;
            case entry_kind::ASTERISK:
               throw pegtl::parse_error( "asterisk could not be expanded", e.get_asterisk().position );  // Can happen when there are also unresolved references.
            case entry_kind::REFERENCE:
               throw pegtl::parse_error( "reference '" + e.get_reference().to_string() + "' could not be resolved", e.get_reference().at( 0 ).position );
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
