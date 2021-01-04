// Copyright (c) 2018-2020 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_INTERNAL_CONFIG_ACTION_HPP
#define TAO_CONFIG_INTERNAL_CONFIG_ACTION_HPP

#include <cassert>

#include "config_grammar.hpp"
#include "constants.hpp"
#include "entry_kind.hpp"
#include "extension_maps.hpp"
#include "json.hpp"
#include "key1_guard.hpp"
#include "pegtl.hpp"
#include "phase1_append.hpp"

namespace tao::config::internal
{
   template< typename Rule >
   struct config_action
      : pegtl::nothing< Rule >
   {};

   template<>
   struct config_action< rules::permanent >
   {
      template< typename State >
      static void apply0( State& st, const extension_maps& )
      {
         phase1_append( st.root, st.prefix + st.suffix, permanent );
      }
   };

   template<>
   struct config_action< rules::temporary >
   {
      template< typename State >
      static void apply0( State& st, const extension_maps& )
      {
         phase1_append( st.root, st.prefix + st.suffix, temporary );
      }
   };

   template<>
   struct config_action< rules::assign_head >
   {
      template< typename State >
      static void apply0( State& st, const extension_maps& )
      {
         phase1_append( st.root, st.prefix + st.suffix, entry_remove );
      }
   };

   template<>
   struct config_action< json::jaxn::internal::rules::begin_array >
   {
      template< typename State >
      static void apply0( State& st, const extension_maps& )
      {
         phase1_append( st.root, st.prefix + st.suffix, entry_kind::array );
      }
   };

   template<>
   struct config_action< json::jaxn::internal::rules::begin_object >
   {
      template< typename State >
      static void apply0( State& st, const extension_maps& )
      {
         phase1_append( st.root, st.prefix + st.suffix, entry_kind::object );
      }
   };

   template<>
   struct config_action< rules::value_list >
   {
      template< typename State >
      static void apply0( State& st, const extension_maps& )
      {
         assert( !st.suffix.empty() );

         st.suffix.back().set_generation( ++st.generation );
      }
   };

   template<>
   struct config_action< rules::element_list >
      : pegtl::instantiate< key1_guard >
   {};

}  // namespace tao::config::internal

#endif
