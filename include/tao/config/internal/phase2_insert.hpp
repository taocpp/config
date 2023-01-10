// Copyright (c) 2018-2023 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_INTERNAL_PHASE2_INSERT_HH
#define TAO_CONFIG_INTERNAL_PHASE2_INSERT_HH

#include <utility>

#include "concat.hpp"
#include "forward.hpp"
#include "object.hpp"

namespace tao::config::internal
{
   inline void phase2_insert( object&& l, object& r )
   {
      for( std::pair< const std::string, concat >& m : l.object ) {
         const auto pair = r.object.try_emplace( m.first, m.second );
         if( !pair.second ) {
            if( pair.first->second.remove ) {
               continue;
            }
            if( m.second.remove ) {
               pair.first->second.remove = true;
            }
            if( m.second.temporary ) {
               pair.first->second.temporary = true;
            }
            pair.first->second.concat.splice( pair.first->second.concat.begin(), m.second.concat );
         }
      }
   }

}  // namespace tao::config::internal

#endif
