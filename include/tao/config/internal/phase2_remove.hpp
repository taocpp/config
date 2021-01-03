// Copyright (c) 2019-2020 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_INTERNAL_PHASE2_REMOVE_HPP
#define TAO_CONFIG_INTERNAL_PHASE2_REMOVE_HPP

#include <cassert>

#include "array.hpp"
#include "concat.hpp"
#include "entry.hpp"
#include "forward.hpp"
#include "object.hpp"
#include "pegtl.hpp"

namespace tao::config::internal
{
   inline void phase2_remove( array& a );
   inline void phase2_remove( object& o );

   inline void phase2_remove( concat& c )
   {
      auto i = c.concat.begin();

      while( i != c.concat.end() ) {
         switch( i->kind() ) {
            case entry_kind::value:
               ++i;
               continue;
            case entry_kind::reference:
               throw pegtl::parse_error( "unresolved reference", i->get_reference().at( 0 ).position );
            case entry_kind::array:
               phase2_remove( i->get_array() );
               ++i;
               continue;
            case entry_kind::object:
               phase2_remove( i->get_object() );
               ++i;
               continue;
            case entry_kind::remove:
               i = c.concat.erase( i );
               continue;
         }
         assert( false );  // UNREACHABLE
      }
      if( c.concat.size() > 1 ) {
         throw pegtl::parse_error( "concat size greater than one", c.position );
      }
   }

   inline void phase2_remove( array& a )
   {
      auto i = a.array.begin();

      while( i != a.array.end() ) {
         phase2_remove( *i );
         if( i->temporary || i->concat.empty() ) {
            i = a.array.erase( i );
         }
         else {
            ++i;
         }
      }
   }

   inline void phase2_remove( object& o )
   {
      auto i = o.object.begin();

      while( i != o.object.end() ) {
         phase2_remove( i->second );
         if( i->second.temporary || i->second.concat.empty() ) {
            i = o.object.erase( i );
         }
         else {
            ++i;
         }
      }
   }

}  // namespace tao::config::internal

#endif

