// Copyright (c) 2018-2019 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_INTERNAL_STATE_HPP
#define TAO_CONFIG_INTERNAL_STATE_HPP

#include <functional>
#include <string>
#include <vector>

#include "../key.hpp"

#include "entry.hpp"
#include "forward.hpp"
#include "json.hpp"
#include "pegtl.hpp"

namespace tao::config::internal
{
   struct state
   {
      state()
         : root( nullptr, pegtl::position( pegtl::internal::iterator(), "(root)" ) ),
           temporary( json::uninitialized, pegtl::position( pegtl::internal::iterator(), "(temporary)" ) )
      {
         root.set_object();
         ostack.emplace_back( &root );
      }

      entry root;

      // General Structure

      bool clear = false;  // 'true' when = was used for something new.

      std::vector< entry* > ostack;   // Object contexts via '{'.
      std::vector< concat* > lstack;  // Current rules::value_list.
      std::vector< entry* > astack;   // Array contexts via '['.

      // Phase 1 Extensions

      json_t temporary;
      std::string extension;

      // Phase 2 Extensions

      std::vector< key > temporaries;  // Delete from final result.
      std::vector< json_t* > rstack;   // Nested phase 2 references (and also phase 1 keys).
   };

}  // namespace tao::config::internal

#endif
