// Copyright (c) 2018-2024 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_INTERNAL_CONFIG_ACTION_HPP
#define TAO_CONFIG_INTERNAL_CONFIG_ACTION_HPP

#include <cassert>
#include <cerrno>
#include <string>
#include <system_error>
#include <utility>
#include <vector>

#include "concat.hpp"
#include "config_grammar.hpp"
#include "constants.hpp"
#include "entry_kind.hpp"
#include "forward.hpp"
#include "json.hpp"
#include "key1.hpp"
#include "key1_action.hpp"
#include "key1_guard.hpp"
#include "pegtl.hpp"
#include "phase1_append.hpp"
#include "string_utility.hpp"

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
      static void apply0( State& st, const function_map& /*unused*/ )
      {
         const auto f = []( concat& c ) { c.concat.clear(); c.remove = true; };
         phase1_append( st.root, st.prefix + st.suffix, f, phase1_mode::implicit );
      }
   };

   template<>
   struct config_action< json::jaxn::internal::rules::begin_array >
   {
      template< typename Input, typename State >
      static void apply( Input& in, State& st, const function_map& /*unused*/ )
      {
         const auto p = in.position();
         const auto f = [ & ]( concat& c ) { c.back_ensure_init( array_init, p ); };
         phase1_append( st.root, st.prefix + st.suffix, f, phase1_mode::manifest );
      }
   };

   template<>
   struct config_action< json::jaxn::internal::rules::begin_object >
   {
      template< typename Input, typename State >
      static void apply( Input& in, State& st, const function_map& /*unused*/ )
      {
         const auto p = in.position();
         const auto f = [ & ]( concat& c ) { c.back_ensure_init( object_init, p ); };
         phase1_append( st.root, st.prefix + st.suffix, f, phase1_mode::manifest );
      }
   };

   template<>
   struct config_action< rules::function_name >
   {
      template< typename Input, typename State >
      static void apply( Input& in, State& st, const function_map& /*unused*/ )
      {
         const auto s = in.string();
         const auto p = in.position();
         const auto f = [ & ]( concat& c ) { c.back_emplace_func( s, p ); };
         phase1_append( st.root, st.prefix + st.suffix, f, phase1_mode::manifest );
      }
   };

   template<>
   struct config_action< rules::value_list >
   {
      template< typename State >
      static void apply0( State& st, const function_map& /*unused*/ )
      {
         assert( !st.suffix.empty() );

         st.suffix.back().set_generation( ++st.generation );
      }
   };

   template<>
   struct config_action< rules::element_list >
      : pegtl::instantiate< key1_guard >
   {};

   template<>
   struct config_action< rules::extension_list >
      : pegtl::instantiate< key1_guard >
   {};

   template<>
   struct config_action< rules::key1_rule >
      : pegtl::change_action_and_state< key1_action, std::vector< key1_part > >
   {
      template< typename Input, typename State >
      static void success( const Input& /*unused*/, std::vector< key1_part >& s, State& st, const function_map& /*unused*/ )
      {
         assert( st.member.empty() );
         std::swap( st.member.vector(), s );
      }
   };

   template<>
   struct config_action< rules::temporary_extension >
   {
      template< typename State >
      static void apply0( State& st, const function_map& /*unused*/ )
      {
         assert( !st.member.empty() );
         const auto f = []( concat& c ) { c.temporary = true; };
         phase1_append( st.root, st.prefix + st.suffix + st.member, f, phase1_mode::implicit );
         st.member.vector().clear();
      }
   };

   template<>
   struct config_action< rules::permanent_extension >
   {
      template< typename State >
      static void apply0( State& st, const function_map& /*unused*/ )
      {
         assert( !st.member.empty() );
         const auto f = []( concat& c ) { c.temporary = false; };
         phase1_append( st.root, st.prefix + st.suffix + st.member, f, phase1_mode::implicit );
         st.member.vector().clear();
      }
   };

   template<>
   struct config_action< rules::include >
   {
      template< typename State >
      static void apply0( State& st, const function_map& /*unused*/ )
      {
         st.include_is_optional = false;
      }
   };

   template<>
   struct config_action< rules::inc_if >
   {
      template< typename State >
      static void apply0( State& st, const function_map& /*unused*/ )
      {
         st.include_is_optional = true;
      }
   };

   template<>
   struct config_action< rules::inc_fn >
   {
      template< typename Input, typename State >
      static void apply( const Input& ai, State& st, const function_map& fm )
      {
         try {
            pegtl::file_input in( ai.string() );
            pegtl::parse_nested< rules::config_file, config_action >( ai.position(), static_cast< pegtl_input_t& >( in ), st, fm );
         }
         catch( const std::system_error& e ) {
            if( !st.include_is_optional ) {
               throw pegtl::parse_error( strcat( "include error: ", e.what() ), ai.position() );
            }
            if( e.code().value() != ENOENT ) {
               throw pegtl::parse_error( strcat( "include optional error: ", e.what() ), ai.position() );
            }
         }
      }
   };

}  // namespace tao::config::internal

#endif
