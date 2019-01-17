// Copyright (c) 2018-2019Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_INTERNAL_STATE_HPP
#define TAO_CONFIG_INTERNAL_STATE_HPP

#include <functional>
#include <string>
#include <vector>

#include "entry.hpp"
#include "json.hpp"
#include "pegtl.hpp"

namespace tao
{
   namespace config
   {
      namespace internal
      {
         struct state
         {
            state()
               : root( entry::make_object( nullptr, internal::position( pegtl::internal::iterator(), "(implicit)" ) ) )
            {
               ostack.emplace_back( &root );
            }

            entry root;

            // General Structure

            bool clear_for_assign = false;

            std::vector< entry* > ostack;  // Object contexts via '{'
            std::vector< concat* > lstack;  // Current rules::value_list
            std::vector< entry* > astack;  // Array contexts via '['

            // Phase 1 Extensions

            json::value temporary;
            std::string extension;

            // Phase 2 Extensions

            std::vector< json::value* > rstack;  // Nested phase 2 references (and also phase 1 keys).
         };

      }  // namespace internal

   }  // namespace config

}  // namespace tao

#endif
