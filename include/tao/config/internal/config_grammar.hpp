// Copyright (c) 2018-2023 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_INTERNAL_CONFIG_GRAMMAR_HPP
#define TAO_CONFIG_INTERNAL_CONFIG_GRAMMAR_HPP

#include "extension_maps.hpp"
#include "forward.hpp"
#include "json.hpp"
#include "key1.hpp"
#include "key1_grammar.hpp"
#include "key1_guard.hpp"
#include "member_extensions.hpp"
#include "parse_utility.hpp"
#include "pegtl.hpp"
#include "phase1_append.hpp"
#include "reference2.hpp"
#include "value_extensions.hpp"

namespace tao::config::internal::rules
{
   struct reference_value
   {
      using rule_t = reference_value;
      using subs_t = pegtl::type_list< reference2_rest >;

      template< pegtl::apply_mode A,
                pegtl::rewind_mode M,
                template< typename... >
                class Action,
                template< typename... >
                class Control,
                typename State >
      [[nodiscard]] static bool match( pegtl_input_t& in, State& st, const extension_maps& )
      {
         const auto r = parse_reference2( in );
         const auto f = [ & ]( concat& c ) { c.concat.emplace_back( r ); };
         phase1_append( st.root, st.prefix + st.suffix, f, phase1_mode::manifest );
         return true;
      }
   };

   struct jaxn_value
   {
      using rule_t = jaxn_value;
      using subs_t = pegtl::type_list< json::jaxn::internal::rules::sor_single_value >;

      template< pegtl::apply_mode A,
                pegtl::rewind_mode M,
                template< typename... >
                class Action,
                template< typename... >
                class Control,
                typename State >
      [[nodiscard]] static bool match( pegtl_input_t& in, State& st, const extension_maps& )
      {
         const auto j = parse_jaxn( in );
         const auto f = [ & ]( concat& c ) { c.concat.emplace_back( j ); };
         phase1_append( st.root, st.prefix + st.suffix, f, phase1_mode::manifest );
         return true;
      }
   };

   // clang-format off
   struct extension_value : pegtl::must< pegtl::function< do_value_extension >, pegtl::one< ')' > > {};
   struct bracketed_value : pegtl::if_must< pegtl::one< '(' >, pegtl::if_must_else< pegtl::at< reference2_rest >, reference_value, extension_value > > {};

   struct array;
   struct object;

   struct remove : pegtl::keyword< 'd', 'e', 'l', 'e', 't', 'e' > {};

   struct value_part : pegtl::sor< array, object, bracketed_value, jaxn_value > {};
   struct value_list : pegtl::list< value_part, pegtl::one< '+' >, jaxn::ws > {};

   struct assign_head : pegtl::one< ':', '=' > {};
   struct assign_member : pegtl::if_must< assign_head, wss, pegtl::sor< remove, value_list > > {};

   struct append_head : pegtl::sor< pegtl::string< '+', '=' >, pegtl::at< pegtl::one< '{', '[' > > > {};
   struct append_member : pegtl::if_must< append_head, wss, value_list > {};

   struct key_member : pegtl::sor< assign_member, append_member > {};

   struct member_key
   {
      using rule_t = member_key;
      using subs_t = pegtl::type_list< key1_rule, pegtl::must< wss, key_member > >;

      template< pegtl::apply_mode A,
                pegtl::rewind_mode M,
                template< typename... >
                class Action,
                template< typename... >
                class Control,
                typename State >
      [[nodiscard]] static bool match( pegtl_input_t& in, State& st, const extension_maps& em )
      {
         const key1_guard kg( st, parse_key1( in ) );
         return Control< pegtl::must< wss, key_member > >::template match< A, M, Action, Control >( in, st, em );
      }
   };

   struct extension_member : pegtl::if_must< pegtl::one< '(' >, pegtl::function< do_member_extension >, pegtl::one< ')' > > {};

   struct member : pegtl::sor< extension_member, member_key > {};

   struct opt_comma : pegtl::opt< pegtl::one< ',' >, wss > {};

   template< typename U > struct member_list_impl : pegtl::until< U, member, wss, opt_comma > {};

   struct element_list : pegtl::until< jaxn::end_array, value_list, wss, opt_comma > {};
   struct array : pegtl::if_must< jaxn::begin_array, element_list > {};
   struct member_list : member_list_impl< jaxn::end_object > {};
   struct object : pegtl::if_must< jaxn::begin_object, member_list > {};
   struct compat_file : pegtl::must< member_list, wss, pegtl::eof > {};
   struct config_list : member_list_impl< pegtl::eof > {};
   struct begin_compat : pegtl::disable< jaxn::begin_object > {};
   struct config_file : pegtl::must< wss, pegtl::if_must_else< begin_compat, compat_file, config_list > > {};
   struct value : pegtl::must< wss, value_part, wss, pegtl::eof > {};
   // clang-format on

}  // namespace tao::config::internal::rules

#endif
