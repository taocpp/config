// Copyright (c) 2018-2020 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_INTERNAL_ACTION_HPP
#define TAO_CONFIG_INTERNAL_ACTION_HPP

#include <cassert>

#include "entry_kind.hpp"
#include "grammar.hpp"
#include "json.hpp"
#include "key1_guard.hpp"
#include "pegtl.hpp"
#include "phase1_append.hpp"

namespace tao::config::internal
{
   template< typename Rule >
   struct action
      : pegtl::nothing< Rule >
   {};

   template<>
   struct action< rules::erase >
   {
      template< typename State >
      static void apply0( State& st )
      {
         //         phase1_erase( st.root, st.prefix + st.suffix );
      }
   };

   template<>
   struct action< json::jaxn::internal::rules::begin_array >
   {
      template< typename State >
      static void apply0( State& st )
      {
         phase1_append( st.root, st.prefix + st.suffix, entry_kind::array );
      }
   };

   template<>
   struct action< json::jaxn::internal::rules::begin_object >
   {
      template< typename State >
      static void apply0( State& st )
      {
         phase1_append( st.root, st.prefix + st.suffix, entry_kind::object );
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
      : pegtl::instantiate< key1_guard >
   {};

}  // namespace tao::config::internal

#endif
