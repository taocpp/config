// Copyright (c) 2018-2023 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_INTERNAL_CONFIG_ACTION_HPP
#define TAO_CONFIG_INTERNAL_CONFIG_ACTION_HPP

#include <cassert>

#include "concat.hpp"
#include "config_grammar.hpp"
#include "constants.hpp"
#include "entry_kind.hpp"
#include "extension_maps.hpp"
#include "forward.hpp"
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
   struct config_action< rules::assign_head >
   {
      template< typename State >
      static void apply0( State& st, const extension_maps& )
      {
         const auto f = []( concat& c ) { c.concat.clear(); c.remove = true; };
         phase1_append( st.root, st.prefix + st.suffix, f, phase1_mode::implicit );
      }
   };

   template<>
   struct config_action< json::jaxn::internal::rules::begin_array >
   {
      template< typename Input, typename State >
      static void apply( Input& in, State& st, const extension_maps& )
      {
         const auto p = in.position();
         const auto f = [ & ]( concat& c ) { c.back_ensure_kind( entry_kind::array, p ); };
         phase1_append( st.root, st.prefix + st.suffix, f, phase1_mode::manifest );
      }
   };

   template<>
   struct config_action< json::jaxn::internal::rules::begin_object >
   {
      template< typename Input, typename State >
      static void apply( Input& in, State& st, const extension_maps& )
      {
         const auto p = in.position();
         const auto f = [ & ]( concat& c ) { c.back_ensure_kind( entry_kind::object, p ); };
         phase1_append( st.root, st.prefix + st.suffix, f, phase1_mode::manifest );
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
