// Copyright (c) 2018-2020 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_INTERNAL_GRAMMAR_HPP
#define TAO_CONFIG_INTERNAL_GRAMMAR_HPP

#include "forward.hpp"
#include "json.hpp"
#include "key1.hpp"
#include "key1_guard.hpp"
#include "key1_rules.hpp"
#include "parse_utility.hpp"
#include "pegtl.hpp"
#include "phase1_append.hpp"
#include "ref2.hpp"

namespace tao::config::internal::rules
{
   struct ref_value
   {
      using rule_t = ref_value;
      using subs_t = pegtl::type_list< ref2_rest >;

      template< pegtl::apply_mode A,
                pegtl::rewind_mode M,
                template< typename... >
                class Action,
                template< typename... >
                class Control,
                typename State >
      [[nodiscard]] static bool match( pegtl_input_t& in, State& st )
      {
         const ref2 r = parse_ref2( in );
         phase1_append( st.root, st.prefix + st.suffix, r );
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
      [[nodiscard]] static bool match( pegtl_input_t& in, State& st )
      {
         const json_t j = parse_jaxn( in );
         phase1_append( st.root, st.prefix + st.suffix, j );
         return true;
      }
   };

   namespace jaxn = tao::json::jaxn::internal::rules;

   // clang-format off
   struct wss : pegtl::star< jaxn::ws > {};
   struct wsp : pegtl::plus< jaxn::ws > {};

   struct ext_name : pegtl::seq< ident, pegtl::opt< pegtl::one< '?' > > > {};
   //   struct ext_value_func : pegtl::function< do_value_extension > {};
   struct ext_value_func : pegtl::one< '%' > {};  // TODO: Remove this placeholder.
   struct ext_value : pegtl::must< ext_value_func, pegtl::one< ')' > > {};

   struct bra_value : pegtl::if_must< pegtl::one< '(' >, pegtl::if_must_else< pegtl::at< ref2_rest >, ref_value, ext_value > > {};

   struct inner_ext : pegtl::if_must< pegtl::one< '(' >, ext_name, wsp > {};
   struct outer_ext : pegtl::must< ext_name, wsp > {};

   struct array;
   struct object;

   struct remove : pegtl::string< 'd', 'e', 'l', 'e', 't', 'e' > {};

   struct value_part : pegtl::sor< array, object, bra_value, jaxn_value > {};
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
      [[nodiscard]] static bool match( pegtl_input_t& in, State& st )
      {
         const key1 k = parse_key1( in );
         const key1_guard kg( st, k );
         return Control< pegtl::must< wss, key_member > >::template match< A, M, Action, Control >( in, st );
      }
   };

   //   struct ext_member_func : pegtl::function< do_member_extension > {};
   struct ext_member_func : pegtl::one< '%' > {};  // TODO: Remove this placeholder.
   struct ext_member : pegtl::if_must< pegtl::one< '(' >, ext_member_func, pegtl::one< ')' > > {};

   struct member : pegtl::sor< ext_member, member_key > {};

   struct opt_comma : pegtl::opt< pegtl::one< ',' >, wss > {};

   template< typename U > struct member_list_impl : pegtl::until< U, member, wss, opt_comma > {};

   struct element_list : pegtl::until< jaxn::end_array, value_list, wss, opt_comma > {};
   struct array : pegtl::if_must< jaxn::begin_array, element_list > {};
   struct member_list : member_list_impl< jaxn::end_object > {};
   struct object : pegtl::if_must< jaxn::begin_object, member_list > {};
   struct compat_file : pegtl::must< member_list, wss, pegtl::eof > {};
   struct config_list : member_list_impl< pegtl::eof > {};
   struct config_file : pegtl::must< wss, pegtl::if_must_else< jaxn::begin_object, compat_file, config_list > > {};
   // clang-format on

}  // namespace tao::config::internal::rules

#endif
