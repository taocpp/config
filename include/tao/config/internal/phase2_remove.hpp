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
   void phase2_concat_remove( concat& c )
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
               for( auto& d : i->get_array().array ) {
                  phase2_concat_remove( d );
               }
               ++i;
               continue;
            case entry_kind::object:
               for( auto& p : i->get_object().object ) {
                  phase2_concat_remove( p.second );
               }
               ++i;
               continue;
            case entry_kind::remove:
               i = c.concat.erase( i );
               continue;
         }
         assert( false );  // UNREACHABLE
      }
      if( c.concat.size() != 1 ) {
         throw pegtl::parse_error( "concat size not one", c.position );
      }
   }

   void phase2_remove( object& o )
   {
      for( auto& p : o.object ) {
         phase2_concat_remove( p.second );
      }
   }

}  // namespace tao::config::internal

#endif

