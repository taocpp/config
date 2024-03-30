// Copyright (c) 2018-2024 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_INTERNAL_CONFIG_GRAMMAR_HPP
#define TAO_CONFIG_INTERNAL_CONFIG_GRAMMAR_HPP

#include "forward.hpp"
#include "jaxn_action.hpp"
#include "jaxn_to_entry.hpp"
#include "json.hpp"
#include "key1_grammar.hpp"
#include "key1_guard.hpp"
#include "parse_utility.hpp"
#include "pegtl.hpp"
#include "phase1_append.hpp"
#include "reference2.hpp"

namespace tao::config::internal::rules
{
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
      [[nodiscard]] static bool match( pegtl_input_t& in, State& st, const function_map& /*unused*/ )
      {
         jaxn_to_entry consumer;
         pegtl::parse< pegtl::must< json::jaxn::internal::rules::sor_single_value >, jaxn_action, json::jaxn::internal::errors >( in, consumer );
         // assert( consumer.value.has_value() );
         const auto f = [ &e = *consumer.value ]( concat& c ) { c.concat.emplace_back( std::move( e ) ); };
         phase1_append( st.root, st.prefix + st.suffix, f, phase1_mode::manifest );
         return true;
      }
   };

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
      [[nodiscard]] static bool match( pegtl_input_t& in, State& st, const function_map& /*unused*/ )
      {
         const auto r = parse_reference2( in );
         const auto f = [ & ]( concat& c ) { c.concat.emplace_back( r ); };
         phase1_append( st.root, st.prefix + st.suffix, f, phase1_mode::manifest );
         return true;
      }
   };

   struct array;
   struct object;

   struct extension_list;

   // clang-format off
   struct remove : pegtl::keyword< 'd', 'e', 'l', 'e', 't', 'e' > {};

   struct function_name : pegtl::seq< ident, pegtl::opt< pegtl::one< '?' > > > {};
   struct extension_value : pegtl::seq< function_name, wsp, extension_list > {};
   struct bracketed_value : pegtl::if_must< pegtl::one< '(' >, wss, pegtl::if_must_else< pegtl::at< reference2_rest >, reference_value, extension_value > > {};

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
      [[nodiscard]] static bool match( pegtl_input_t& in, State& st, const function_map& fm )
      {
         const key1_guard kg( st, parse_key1( in ) );
         return Control< pegtl::must< wss, key_member > >::template match< A, M, Action, Control >( in, st, fm );
      }
   };

   struct inc_if : pegtl::one< '?' > {};
   struct opt_if : pegtl::opt< inc_if > {};
   struct inc_fn : pegtl::plus< pegtl::not_one< '"' > > {};  // TODO: Do we want/need to allow JSON escape sequences in filenames?

   struct include : pegtl::string< 'i', 'n', 'c', 'l', 'u', 'd', 'e' > {};
   struct permanent : pegtl::string< 'p', 'e', 'r', 'm', 'a', 'n', 'e', 'n', 't' > {};
   struct temporary : pegtl::string< 't', 'e', 'm', 'p', 'o', 'r', 'a', 'r', 'y' > {};

   struct filename : pegtl::seq< pegtl::one< '"' >, inc_fn, pegtl::one< '"' > > {};

   struct include_extension : pegtl::if_must< include, opt_if, wsp, filename > {};
   struct permanent_extension : pegtl::if_must< permanent, wsp, key1_rule > {};
   struct temporary_extension : pegtl::if_must< temporary, wsp, key1_rule > {};

   struct extension_member : pegtl::if_must< pegtl::one< '(' >, wss, pegtl::sor< include_extension, temporary_extension, permanent_extension >, wss, pegtl::one< ')' > > {};

   struct member : pegtl::sor< extension_member, member_key > {};

   struct opt_comma : pegtl::opt< pegtl::one< ',' >, wss > {};

   template< typename U > struct member_list_impl : pegtl::until< U, member, wss, opt_comma > {};
   template< typename U > struct element_list_impl : pegtl::until< U, value_list, wss, opt_comma > {};

   struct element_list : element_list_impl< jaxn::end_array > {};
   struct extension_list : element_list_impl< pegtl::one< ')' > > {};
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
