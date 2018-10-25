// Copyright (c) 2018 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_INTERNAL_STATE_HPP
#define TAO_CONFIG_INTERNAL_STATE_HPP

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

            std::vector< concat* > lstack;  // Current rules::value_list
            std::vector< array_t* > astack;  // Array contexts via '['
            std::vector< object_t* > ostack;  // Object contexts via '{'

            // Phase 1 Extensions

            bool alternative = false;  // TODO: Make less hackish?

            pointer key;  // TODO: PEGTL switching-style?
            std::string str;  // TODO: PEGTL switching-style?
         };

      }  // namespace internal

   }  // namespace config

}  // namespace tao

#endif
