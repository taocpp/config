// Copyright (c) 2020-2024 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_INTERNAL_PHASE2_EVERYTHING_HPP
#define TAO_CONFIG_INTERNAL_PHASE2_EVERYTHING_HPP

#include "forward.hpp"
#include "phase2_additions.hpp"
#include "phase2_asterisks.hpp"
#include "phase2_functions.hpp"
#include "phase2_references.hpp"
#include "state.hpp"

namespace tao::config::internal
{
   [[nodiscard]] inline bool phase2_iteration( state& st, const function_map& fm )
   {
      return ( phase2_functions( st, fm ) | phase2_additions( st.root ) | phase2_references( st.root ) | phase2_asterisks( st.root ) ) > 0;
   }

   inline void phase2_everything( state& st, const function_map& fm )
   {
      while( phase2_iteration( st, fm ) ) {
         // This loop could do with some major optimisations; probably not worth the effort for config files.
      }
   }

}  // namespace tao::config::internal

#endif
