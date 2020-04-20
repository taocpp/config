// Copyright (c) 2018-2020 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_INTERNAL_GRAMMAR_HPP
#define TAO_CONFIG_INTERNAL_GRAMMAR_HPP

#include "json.hpp"
#include "pegtl.hpp"

namespace tao::config::internal::rules
{
   namespace jaxn = tao::json::jaxn::internal::rules;

   // NOTE: Some rules are defined multiple times with different names in
   // order to provide anchor points for different PEGTL actions without
   // having to use what the PEGTL documentation calls "switching style".

   using ws1 = jaxn::ws;  // Also handles comments.

   // clang-format off
   struct wss : pegtl::star< ws1 > {};
   struct wsp : pegtl::plus< ws1 > {};

   struct dot : pegtl::one< '.' > {};
   struct plus : pegtl::one< '+' > {};
   struct star : pegtl::one< '*' > {};
   struct minus : pegtl::one< '-' > {};
   struct comma : pegtl::one< ',' > {};
   struct dollar : pegtl::one< '$' > {};
   struct equals : pegtl::one< ':', '=' > {};
   struct plus_equals : pegtl::string< '+', '=' > {};

   struct quote_2 : pegtl::one< '"' > {};
   struct curly_a : pegtl::one< '{' > {};
   struct curly_z : pegtl::one< '}' > {};
   struct round_a : pegtl::one< '(' > {};
   struct round_z : pegtl::one< ')' > {};
   struct square_a : pegtl::one< '[' > {};
   struct square_z : pegtl::one< ']' > {};
   struct question : pegtl::one< '?' > {};

   struct null_s : pegtl::string< 'n', 'u', 'l', 'l' > {};
   struct true_s : pegtl::string< 't', 'r', 'u', 'e' > {};
   struct false_s : pegtl::string< 'f', 'a', 'l', 's', 'e' > {};
   struct delete_s : pegtl::string< 'd', 'e', 'l', 'e', 't', 'e' > {};

   struct index : pegtl::plus< pegtl::digit > {};

   struct identifier_first : pegtl::ranges< 'a', 'z', 'A', 'Z', '_' > {};
   struct identifier_other : pegtl::ranges< 'a', 'z', 'A', 'Z', '0', '9', '-', '-', '_' > {};
   struct identifier : pegtl::seq< identifier_first, pegtl::star< identifier_other > > {};

   struct array;
   struct object;

   struct ref_part;
   struct ext_value;
   struct reference;

   struct at_quote : pegtl::at< pegtl::one< '\'', '"' > > {};

   struct opt_comma : pegtl::opt< comma, wss > {};
   struct opt_question : pegtl::opt< question > {};

   struct quoted_choice : jaxn::string_fragment {};
   struct quoted : pegtl::if_must< at_quote, quoted_choice > {};

   struct ref_part : pegtl::sor< identifier, quoted, reference, index, minus > {};
   struct ref_head : pegtl::sor< identifier, quoted, reference > {};
   struct ref_body : pegtl::seq< ref_head, pegtl::star_must< dot, ref_part > > {};
   struct reference : pegtl::if_must< round_a, ref_body, round_z > {};

   struct key_part : pegtl::sor< identifier, quoted, index, minus, star > {};
   struct key_head : pegtl::sor< identifier, quoted > {};
   struct pointer : pegtl::seq< key_head, pegtl::star_must< dot, key_part > > {};

   struct ext_name : pegtl::seq< identifier_first, pegtl::star< identifier_other >, opt_question > {};

   struct ext_value_impl
   {
      using rule_t = ext_value_impl;
      using subs_t = pegtl::empty_list;

      template< pegtl::apply_mode A,
                pegtl::rewind_mode M,
                template< typename... > class Action,
                template< typename... > class Control,
                typename Input,
                typename State >
      static bool match( Input& in, State& st )
      {
         return do_value_extension( in, st );
      }
   };

   struct ext_value : pegtl::if_must< round_a, ext_value_impl, round_z > {};

   struct at_round_a : pegtl::at< round_a > {};
   struct at_ext_value : pegtl::at< round_a, ext_name, ws1 > {};
   struct special_choice : pegtl::if_must_else< at_ext_value, ext_value, reference > {};
   struct special_value : pegtl::if_must< at_round_a, special_choice > {};

   struct binary_choice : pegtl::sor< jaxn::bstring, jaxn::bdirect > {};
   struct binary_value : pegtl::if_must< dollar, binary_choice > {};

   struct string_choice : jaxn::string_fragment {};
   struct string_value : pegtl::if_must< at_quote, string_choice > {};

   struct at_number : pegtl::at< pegtl::one< '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '-', '+', '.', 'I', 'N' > > {};
   struct number_value : pegtl::if_must< at_number, jaxn::sor_value > {};

   struct value_part : pegtl::sor< null_s, true_s, false_s, delete_s, array, object, special_value, string_value, binary_value, number_value > {};
   struct array_part : pegtl::sor< null_s, array, special_value > {};
   struct object_part : pegtl::sor< null_s, object, special_value > {};

   struct element_value_list : pegtl::list< value_part, plus, ws1 > {};
   struct member_value_list : pegtl::list< value_part, plus, ws1 > {};
   struct member_array_list : pegtl::list< array_part, plus, ws1 > {};
   struct member_object_list : pegtl::list< object_part, plus, ws1 > {};
   struct assign_member : pegtl::if_must< equals, wss, member_value_list > {};
   struct append_member : pegtl::if_must< plus_equals, wss, member_value_list > {};
   struct array_member : pegtl::if_must< pegtl::at< square_a >, member_array_list > {};
   struct object_member : pegtl::if_must< pegtl::at< curly_a >, member_object_list > {};
   struct key_member : pegtl::must< pointer, wss, pegtl::sor< assign_member, append_member, array_member, object_member > > {};

   struct ext_member_impl
   {
      using rule_t = ext_member_impl;
      using subs_t = pegtl::empty_list;

      template< pegtl::apply_mode A,
                pegtl::rewind_mode M,
                template< typename... > class Action,
                template< typename... > class Control,
                typename Input,
                typename State >
      static bool match( Input& in, State& st )
      {
         return do_member_extension( in, st );
      }
   };

   struct ext_member : pegtl::if_must< round_a, ext_member_impl, round_z > {};

   struct member : pegtl::sor< ext_member, key_member > {};

   template< typename U > struct member_list_impl : pegtl::until< U, member, wss, opt_comma > {};

   struct element_list : pegtl::until< square_z, element_value_list, wss, opt_comma > {};
   struct member_list : member_list_impl< curly_z > {};
   struct compat_list : member_list_impl< curly_z > {};
   struct grammar_list : member_list_impl< pegtl::eof > {};

   struct array : pegtl::if_must< square_a, wss, element_list > {};
   struct object : pegtl::if_must< curly_a, wss, member_list > {};

   struct compat_file : pegtl::must< wss, compat_list, wss, pegtl::eof > {};
   struct config_file : pegtl::must< wss, pegtl::if_must_else< curly_a, compat_file, grammar_list > > {};

   struct value : pegtl::must< wss, value_part, wss, pegtl::eof > {};
   struct inner : pegtl::if_must< round_a, ext_name, wsp > {};
   struct outer : pegtl::must< ext_name, wsp > {};
   // clang-format on

}  // namespace tao::config::internal::rules

#endif
