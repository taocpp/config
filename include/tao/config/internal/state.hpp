// Copyright (c) 2018 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_INTERNAL_STATE_HPP
#define TAO_CONFIG_INTERNAL_STATE_HPP

#include <functional>
#include <string>
#include <vector>

#include "json.hpp"
#include "pegtl.hpp"
#include "pointer.hpp"
#include "entry.hpp"

namespace tao
{
   namespace config
   {
      namespace internal
      {
         struct state
         {
            state()
            {
               ostack.emplace_back( &result );
            }

            object_t result;

            // General Structure

            bool clear_for_assign = true;

            std::vector< concat* > lstack;  // Current rules::value_list
            std::vector< array_t* > astack;  // Array contexts via '['
            std::vector< object_t* > ostack;  // Object contexts via '{'

            // Phase 1 Extensions

            json::value temporary;
            std::string extension;

            // Phase 2 Extensions

            std::vector< json::value* > rstack;  // Nested phase 2 references (and also phase 1 pointers).
         };

      }  // namespace internal

   }  // namespace config

}  // namespace tao

#endif
