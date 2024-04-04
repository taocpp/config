// Copyright (c) 2018-2024 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_INTERNAL_FUNCTION_IMPLEMENTATIONS_HPP
#define TAO_CONFIG_INTERNAL_FUNCTION_IMPLEMENTATIONS_HPP

#include <cstddef>
#include <string>
#include <utility>
#include <vector>

#include "entry.hpp"
#include "forward.hpp"
#include "jaxn_action.hpp"
#include "jaxn_to_entry.hpp"
#include "json.hpp"
#include "pegtl.hpp"
#include "phase5_repack.hpp"
#include "statistics.hpp"
#include "system_utility.hpp"

#include "../key.hpp"

namespace tao::config::internal
{
   [[nodiscard]] inline binary_t binary_function( const pegtl::position& p, const std::string& s )
   {
      const auto* const d = reinterpret_cast< const std::byte* >( s.data() );
      return binary_t( std::vector< std::byte >( d, d + s.size() ), p );
   }

   [[nodiscard]] inline bool default_function( entry& e )
   {
      array& a = e.get_array();
      if( a.array.size() < 1 ) {
         throw pegtl::parse_error( "default function requires at least one argument", a.position );
      }
      for( concat& c : a.array ) {
         if( c.concat.size() != 1 ) {
            return false;
         }
         entry& f = c.concat.front();

         switch( f.kind() ) {
            case entry_kind::NULL_:
               continue;
            case entry_kind::STRING:
            case entry_kind::BINARY:
            case entry_kind::BOOLEAN:
            case entry_kind::SIGNED:
            case entry_kind::UNSIGNED:
            case entry_kind::DOUBLE:
               break;
            case entry_kind::ARRAY:
               if( !f.get_array().function.empty() ) {
                  return false;
               }
               break;
            case entry_kind::OBJECT:
               break;
            case entry_kind::ASTERISK:
               return false;
            case entry_kind::REFERENCE:
               return false;
         }
         // Both f and a are sub-objects of e.
         entry t = std::move( f );
         e = std::move( t );
         return true;
      }
      throw pegtl::parse_error( "default function requires at least one non-null argument", a.position );
   }

   [[nodiscard]] inline string_t env_function( const pegtl::position& p, const std::string& s )
   {
      return string_t( getenv_throws( p, s ), p );
   }

   [[nodiscard]] inline string_t env_if_function( const pegtl::position& p, const std::string& s, const std::string& d )
   {
      const auto r = getenv_nothrow( s );
      return string_t( r ? ( *r ) : d, p );
   }

   [[nodiscard]] inline entry jaxn_function( const pegtl::position& /*unused*/, const std::string& s )
   {
      jaxn_to_entry consumer;
      pegtl::memory_input in( s, "TODO" );
      pegtl::parse< json::jaxn::internal::grammar, jaxn_action, json::jaxn::internal::errors >( static_cast< pegtl_input_t& >( in ), consumer );
      return std::move( consumer.value ).value();
   }

   [[nodiscard]] inline binary_t read_function( const pegtl::position& p, const std::string& filename )
   {
      const std::string d = read_file_throws( filename );
      const std::byte* x = reinterpret_cast< const std::byte* >( d.data() );
      return binary_t( std::vector< std::byte >( x, x + d.size() ), p );
   }

   [[nodiscard]] inline bool print_function( entry& e )
   {
      array& a = e.get_array();
      if( a.array.size() != 1 ) {
         throw pegtl::parse_error( "print function requires exactly one argument", a.position );
      }
      concat& c = a.array.front();
      if( statistics( c ).is_primitive() ) {
         const tao::json::value v = phase5_repack< json::traits >( c );  // TODO: Optimise away this intermediate data structure.
         std::string s = json::jaxn::to_string( v );
         pegtl::position p = c.position;
         e = entry( string_t( std::move( s ), std::move( p ) ) );
         return true;
      }
      return false;
   }

   [[nodiscard]] inline string_t shell_function( const pegtl::position& p, [[maybe_unused]] const std::string& script )
   {
#if defined( _MSC_VER )
      throw pegtl::parse_error( "shell extension not supported on this platform", p );
#else
      return string_t( shell_popen_throws( p, script ), p );
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
      static void apply( const Input& in, entry& result, const pegtl::position& p )
      {
         concat& c = result.get_array().array.emplace_back( p );
         c.concat.emplace_back( string_t( in.string(), in.position() ) );
      }
   };

   [[nodiscard]] inline entry split_function( const pegtl::position& p, const std::string& s )
   {
      entry result( array_init, p );
      pegtl::memory_input< pegtl::tracking_mode::lazy, pegtl_input_t::eol_t, const char* > in( s, __FUNCTION__ );
      pegtl::parse_nested< split_rule, split_action >( p, in, result, p );
      return result;
   }

   [[nodiscard]] inline string_t string_function( const pegtl::position& p, const std::string& s )
   {
      return string_t( s, p );
   }

}  // namespace tao::config::internal

#endif
