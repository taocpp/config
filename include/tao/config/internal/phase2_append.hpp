// Copyright (c) 2018-2023 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_INTERNAL_PHASE2_APPEND_HH
#define TAO_CONFIG_INTERNAL_PHASE2_APPEND_HH

#include "array.hpp"
#include "forward.hpp"

namespace tao::config::internal
{
   inline void phase2_append( array&& l, array& r )
   {
      r.array.splice( r.array.begin(), l.array );
   }

}  // namespace tao::config::internal

#endif
