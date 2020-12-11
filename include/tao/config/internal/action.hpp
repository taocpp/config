// Copyright (c) 2018-2020 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_INTERNAL_ACTION_HPP
#define TAO_CONFIG_INTERNAL_ACTION_HPP

#include <cassert>

#include "grammar.hpp"
#include "json.hpp"
#include "key1_guard.hpp"
#include "pegtl.hpp"

namespace tao::config::internal
{
   template< typename Rule >
   struct action
      : pegtl::nothing< Rule >
   {};

   template<>
   struct action< json::jaxn::internal::rules::begin_array >
   {
      template< typename State >
      static void apply0( State& st )
      {
         phase1_append( st.root, st.prefix + st.suffix, json::empty_array_t );
      }
   };

   template<>
   struct action< json::jaxn::internal::rules::begin_object >
   {
      template< typename State >
      static void apply0( State& st )
      {
         phase1_append( st.root, st.prefix + st.suffix, json::empty_object_t );
      }
   };

   template<>
   struct action< rules::value_list >
   {
      template< typename State >
      static void apply0( State& st )
      {
         if( st.suffix.back().is_implicit() ) {
            assert( st.suffix.size() == 1 );

            st.suffix.back().inc_index();
         }
      }
   };

   template<>
   struct action< rules::element_list >
      : pegtl::nothing< rules::element_list >
   {
      template< typename Rule,
                pegtl::apply_mode A,
                pegtl::rewind_mode M,
                template< typename... >
                class Action,
                template< typename... >
                class Control,
                typename Input,
                typename State >
      [[nodiscard]] static bool match( Input& in, State& st )
      {
         const key1_guard kg( st, 0 );
         return pegtl::match< rules::element_list, A, M, Action, Control >( in, st );
      }
   };

}  // namespace tao::config::internal

#endif
