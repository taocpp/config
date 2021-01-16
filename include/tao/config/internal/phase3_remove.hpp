// Copyright (c) 2019-2021 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_INTERNAL_PHASE3_REMOVE_HPP
#define TAO_CONFIG_INTERNAL_PHASE3_REMOVE_HPP

#include <cassert>

#include "array.hpp"
#include "concat.hpp"
#include "entry.hpp"
#include "forward.hpp"
#include "object.hpp"
#include "pegtl.hpp"

namespace tao::config::internal
{
   inline void phase3_remove( array& a );
   inline void phase3_remove( object& o );

   inline void phase3_remove( concat& c )
   {
      for( auto& e : c.concat ) {
         switch( e.kind() ) {
            case entry_kind::value:
               continue;
            case entry_kind::reference:
               throw pegtl::parse_error( "unresolved reference", e.get_reference().at( 0 ).position );
            case entry_kind::array:
               phase3_remove( e.get_array() );
               continue;
            case entry_kind::object:
               phase3_remove( e.get_object() );
               continue;
            case entry_kind::concat:
               throw pegtl::parse_error( "unresolved star", e.get_concat().position );  // Can happen when there are also unresolved references.
         }
         assert( false );  // UNREACHABLE
      }
      if( c.concat.size() > 1 ) {
         throw pegtl::parse_error( "concat size greater than one", c.position );  // TODO: Add the positions of all concat entries?
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

