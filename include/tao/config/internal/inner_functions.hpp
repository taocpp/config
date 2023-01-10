// Copyright (c) 2018-2023 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_INTERNAL_INNER_FUNCTIONS_HPP
#define TAO_CONFIG_INTERNAL_INNER_FUNCTIONS_HPP

#include "jaxn_action.hpp"
#include "json.hpp"
#include "json_traits.hpp"
#include "pegtl.hpp"
#include "system_utility.hpp"

namespace tao::config::internal
{
   [[nodiscard]] inline std::vector< std::byte > binary_function( const std::string& s )
   {
      const auto* const p = reinterpret_cast< const std::byte* >( s.data() );
      return std::vector< std::byte >( p, p + s.size() );
   }

   [[nodiscard]] inline json_t cbor_function( const tao::binary_view bv )
   {
      return json::cbor::basic_from_binary< json_traits >( bv );  // TODO: Positions.
   }

   [[nodiscard]] inline json_t default_function( const json_t& l, const json_t& r )
   {
      assert( !l.is_uninitialized() );
      return ( !l.is_null() ) ? l : r;
   }

   [[nodiscard]] inline std::string env_function( const pegtl::position& p, const std::string& s )
   {
      return get_env_throws( p, s );
   }

   [[nodiscard]] inline std::string env_if_function( const std::string& s, const std::string& d )
   {
      const auto r = get_env_nothrow( s );
      return r ? ( *r ) : d;
   }

   [[nodiscard]] inline json_t identity_function( const json_t& j )
   {
      return j;
   }

   [[nodiscard]] inline json_t jaxn_function( const pegtl::position& p, const std::string& s )
   {
      json_to_value consumer( p );
      pegtl::memory_input in( s, "TODO" );
      pegtl::parse< json::jaxn::internal::grammar, jaxn_action, json::jaxn::internal::errors >( static_cast< pegtl_input_t& >( in ), consumer );
      return std::move( consumer.value );
   }

   [[nodiscard]] inline json_t json_function( const std::string& s )
   {
      return json::basic_from_string< json_traits >( s );  // TODO: Positions.
   }

   [[nodiscard]] inline json_t msgpack_function( const tao::binary_view bv )
   {
      return json::msgpack::basic_from_binary< json_traits >( bv );  // TODO: Positions.
   }

   [[nodiscard]] inline std::vector< std::byte > read_function( const std::string& filename )
   {
      const std::string d = read_file_throws( filename );
      const std::byte* p = reinterpret_cast< const std::byte* >( d.data() );
      return std::vector< std::byte >( p, p + d.size() );
   }

   [[nodiscard]] inline std::string shell_function( const pegtl::position& p, [[maybe_unused]] const std::string& script )
   {
#if defined( _MSC_VER )
      throw pegtl::parse_error( "shell extension not supported on this platform", p );
#else
      return shell_popen_throws( p, script );
#endif
   }

   // clang-format off
   struct split_star_ws : pegtl::star< pegtl::space > {};
   struct split_string : pegtl::plus< pegtl::invert< pegtl::space > > {};
   struct split_rule : pegtl::must< split_star_ws, pegtl::star< split_string, split_star_ws >, pegtl::eof > {};
   // clang-format on

   template< typename Rule >
   struct split_action
      : pegtl::nothing< Rule >
   {};

   template<>
   struct split_action< split_string >
   {
      template< typename Input >
      static void apply( const Input& in, std::vector< std::string >& result )
      {
         result.emplace_back( in.string() );
      }
   };

   [[nodiscard]] inline std::vector< std::string > split_function( const pegtl::position& p, const std::string& s )
   {
      std::vector< std::string > result;
      pegtl::memory_input< pegtl::tracking_mode::lazy, pegtl_input_t::eol_t, const char* > in( s, __FUNCTION__ );
      pegtl::parse_nested< split_rule, split_action >( p, in, result );
      return result;
   }

   [[nodiscard]] inline std::string string_function( const pegtl::position& p, const tao::binary_view bv )
   {
      if( !json::internal::validate_utf8_nothrow( std::string_view( reinterpret_cast< const char* >( bv.data() ), bv.size() ) ) ) {
         throw pegtl::parse_error( "invalid utf8 in binary data", p );
      }
      return std::string( reinterpret_cast< const char* >( bv.data() ), bv.size() );
   }

   [[nodiscard]] inline json_t ubjson_function( const tao::binary_view bv )
   {
      return json::ubjson::basic_from_binary< json_traits >( bv );  // TODO: Positions.
   }

}  // namespace tao::config::internal

#endif
