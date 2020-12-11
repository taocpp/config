// Copyright (c) 2018-2020 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_INTERNAL_GRAMMAR_HPP
#define TAO_CONFIG_INTERNAL_GRAMMAR_HPP

#include "forward.hpp"
#include "json.hpp"
#include "parse_utility.hpp"
#include "pegtl.hpp"

namespace tao::config::internal::rules
{
   struct ref_value
   {
      using rule_t = ref_value;
      using subs_t = pegtl::empty_list;

      template< pegtl::apply_mode A,
                pegtl::rewind_mode M,
                template< typename... >
                class Action,
                template< typename... >
                class Control,
                typename State,
                typename Concat >
      [[nodiscard]] static bool match( pegtl_input_t& in, State& st, Concat& cc )
      {
         const auto p = in.position();
         cc.concat.emplace_back( p, parse_ref2( in ) );
         return true;
      }
   };

   struct jaxn_value
   {
      using rule_t = jaxn_value;
      using subs_t = pegtl::empty_list;

      template< pegtl::apply_mode A,
                pegtl::rewind_mode M,
                template< typename... >
                class Action,
                template< typename... >
                class Control,
                typename State,
                typename Concat >
      [[nodiscard]] static bool match( pegtl_input_t& in, State& st, Concat& cc )
      {
         const auto p = in.position();
         cc.concat.emplace_back( p, parse_jaxn( in ) );
         return true;
      }
   };

   namespace jaxn = tao::json::jaxn::internal::rules;

   // clang-format off
   struct wss : pegtl::star< jaxn::ws > {};
   struct wsp : pegtl::plus< jaxn::ws > {};

   struct ext_name : pegtl::seq< ident, pegtl::opt< pegtl::one< '?' > > > {};
   struct ext_value_func : pegtl::function< do_value_extension > {};
   struct ext_value : pegtl::must< ext_value_func, pegtl::one< ')' > > {};

   struct bra_value : pegtl::if_must< pegtl::one< '(' >, pegtl::if_must_else< pegtl::at< ref2_rest >, ref_value, ext_value > > {};

   struct inner_ext : pegtl::if_must< pegtl::one< '(' >, ext_name, wsp > {};
   struct outer_ext : pegtl::must< ext_name, wsp > {};

   struct array;
   struct object;

   struct erase : pegtl::string< 'd', 'e', 'l', 'e', 't', 'e' > {};

   struct value_part : pegtl::sor< array, object, bra_value, jaxn_value > {};
   struct value_list : pegtl::list< value_part, pegtl::one< '+' >, jaxn::ws > {};

   struct key_member
   {
      using rule_t = key_member;
      using subs_t = pegtl::type_list< pegtl::one< '=' >, wss, erase, value_list >;

      template< pegtl::apply_mode A,
                pegtl::rewind_mode M,
                template< typename... >
                class Action,
                template< typename... >
                class Control,
                typename State,
                typename Object >
      [[nodiscard]] static bool match( pegtl_input_t& in, State& st, Object& ob, const key1& k )
      {
         if( in.empty() ) {
            throw pegtl::parse_error( "expected key member value", in );
         }
         const auto p = in.position();
         switch( in.peek_char() ) {
            case ':':
            case '=':
               in.bump();
               skip_ws( in );
               if( pegtl::parse< erase >( in ) ) {


                  return true;
               }
               break;
            case '+':
               in.bump();
               Control< pegtl::must< pegtl::one< '=' >, wss, value_list > >::template match< A, M, Action, Control >( in, k, st );
               return true;
            case '[':
            case '{':


               break;
            default:
               throw pegtl::parse_error( "invalid key member continuation character", in );
         }
         Control< pegtl::must< value_list > >::template match< A, M, Action, Control >( in, k, st );
         return true;
      }
   };

   struct member_key
   {
      using rule_t = member_key;
      using subs_t = pegtl::type_list< pegtl::must< wss, key_member > >;

      template< pegtl::apply_mode A,
                pegtl::rewind_mode M,
                template< typename... >
                class Action,
                template< typename... >
                class Control,
                typename State,
                typename Object >
      [[nodiscard]] static bool match( pegtl_input_t& in, State& st, Object& ob )
      {
         const key1 k = parse_key1( in );
         return Control< pegtl::must< wss, key_member > >::template match< A, M, Action, Control >( in, st, ob, k );
      }
   };

   struct ext_member_func : pegtl::function< do_member_extension > {};
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
