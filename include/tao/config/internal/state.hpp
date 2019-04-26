// Copyright (c) 2018-2019 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_INTERNAL_STATE_HPP
#define TAO_CONFIG_INTERNAL_STATE_HPP

#include <cassert>
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
               : root( nullptr, internal::position( pegtl::internal::iterator(), "(root)" ) )
            {
               root.set_object();
               ostack.emplace_back( &root );
            }

            entry root;

            // General Structure

            std::vector< bool > cstack;  // Clear-for-assign stack.
            std::vector< entry* > ostack;   // Object contexts via '{'.
            std::vector< concat* > lstack;  // Current rules::value_list.
            std::vector< entry* > astack;   // Array contexts via '['.

            // Phase 1 Extensions

            json::value temporary;
            std::string extension;

            // Phase 2 Extensions

            std::vector< json::value* > rstack;  // Nested phase 2 references (and also phase 1 keys).
         };

         inline void apply0_clear( state& st )
         {
            assert( !st.cstack.empty() );
            assert( !st.lstack.empty() );

            if( st.cstack.back() ) {
               st.lstack.back()->back_set_clear();
               st.cstack.back() = false;
            }
         }

      }  // namespace internal

   }  // namespace config

}  // namespace tao

#endif
