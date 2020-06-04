// Copyright (c) 2018-2020 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_INTERNAL_INNER_EXTENSIONS_HPP
#define TAO_CONFIG_INTERNAL_INNER_EXTENSIONS_HPP

#include "action.hpp"
#include "control.hpp"
#include "extension_t.hpp"
#include "extension_utility.hpp"
#include "grammar.hpp"
#include "json.hpp"
#include "key_utility.hpp"
#include "pegtl.hpp"
#include "state.hpp"
#include "system.hpp"

namespace tao::config::internal
{
   inline void do_inner_extension( pegtl_input_t& in, state& st );

   template< typename R, typename A >
   struct extension_wrapper
   {
      static void wrap( const std::function< R( A ) >& f, pegtl_input_t& in, state& st )
      {
         const auto pos = in.position();
         do_inner_extension( in, st );
         const auto arg = st.temporary.as< std::decay_t< A > >();
         st.temporary.assign( f( arg ), pos );
      }
   };

   template< typename A >
   struct extension_wrapper< json_t, A >
   {
      static void wrap( const std::function< json_t( A ) >& f, pegtl_input_t& in, state& st )
      {
         //  const auto pos = in.position();
         do_inner_extension( in, st );
         const auto arg = st.temporary.as< std::decay_t< A > >();
         st.temporary = f( arg );  // TODO: Should top-level position be pos?
      }
   };

   template< typename A >
   struct extension_wrapper< std::string, A >
   {
      static void wrap( const std::function< std::string( A ) >& f, pegtl_input_t& in, state& st )
      {
         const auto pos = in.position();
         do_inner_extension( in, st );
         const auto arg = st.temporary.as< std::decay_t< A > >();
         std::string res = f( arg );
         if( !json::internal::validate_utf8_nothrow( res ) ) {
            throw pegtl::parse_error( "invalid utf-8 in extension result", pos );  // TODO: Name of extension?
         }
         st.temporary.assign( std::move( res ), pos );
      }
   };

   template< typename R, typename A >
   extension_t make_extension( R ( *f )( A ) )
   {
      static_assert( !std::is_reference_v< R > );

      return [ = ]( pegtl_input_t& in, state& st ) {
         extension_wrapper< R, A >::wrap( f, in, st );
      };
   }

   template< typename R, typename A >
   extension_t make_extension( std::function< R( A ) > f )
   {
      static_assert( !std::is_reference_v< R > );

      return [ = ]( pegtl_input_t& in, state& st ) {
         extension_wrapper< R, A >::wrap( f, in, st );
      };
   }

   inline std::vector< std::byte > binary_function( const std::string_view sv )
   {
      const auto* const sp = reinterpret_cast< const std::byte* >( sv.data() );
      return std::vector< std::byte >( sp, sp + sv.size() );
   }

   inline json_t cbor_function( const tao::binary_view bv )
   {
      return json::cbor::basic_from_binary< value_traits >( bv );  // TODO: Positions.
   }

   inline void env_extension( pegtl_input_t& in, state& st )
   {
      const auto pos = in.position();

      do_inner_extension( in, st );

      if( st.temporary.is_string_type() ) {
         const std::string n = st.temporary.as< std::string >();
         std::string v = get_env_throws( pos, n );
         if( !json::internal::validate_utf8_nothrow( v ) ) {
            throw pegtl::parse_error( format( __FILE__, __LINE__, "invalid utf-8 in environment variable", { { "name", n } } ), pos );
         }
         st.temporary.assign( std::move( v ), pos );
         return;
      }
      throw pegtl::parse_error( format( __FILE__, __LINE__, "require string to for environment variable lookup", { st.temporary.type() } ), pos );
   }

   inline void env_if_extension( pegtl_input_t& in, state& st )
   {
      const auto pos = in.position();
      const auto v = obtain_string( in );
      const auto e = get_env_nothrow( v );

      if( pegtl::parse< rules::wsp >( in ) ) {
         const auto d = obtain_string( in );
         st.temporary.assign( e.value_or( d ), pos );

         if( !json::internal::validate_utf8_nothrow( st.temporary.as< std::string >() ) ) {
            throw pegtl::parse_error( format( __FILE__, __LINE__, "invalid utf-8 in environment variable", { { "name", v } } ), pos );
         }
         return;
      }
      st.temporary.assign( e, pos );
   }

   inline json_t jaxn_function( const std::string_view sv )
   {
      return json::jaxn::basic_from_string< value_traits >( sv );  // TODO: Positions.
   }

   inline json_t json_function( const std::string_view sv )
   {
      return json::basic_from_string< value_traits >( sv );  // TODO: Positions.
   }

   inline json_t msgpack_function( const tao::binary_view bv )
   {
      return json::msgpack::basic_from_binary< value_traits >( bv );  // TODO: Positions.
   }

   inline void parse_extension( pegtl_input_t& in, state& st )
   {
      assert( !st.lstack.empty() );

      const auto pos = in.position();

      do_inner_extension( in, st );

      if( st.temporary.is_string_type() ) {
         pegtl::string_input< pegtl::tracking_mode::eager, pegtl_input_t::eol_t > i2( st.temporary.as< std::string >(), __FUNCTION__ );
         pegtl::parse_nested< rules::value, action, control >( in, static_cast< pegtl_input_t& >( i2 ), st );
         return;
      }
      throw pegtl::parse_error( format( __FILE__, __LINE__, "require string to parse value", { st.temporary.type() } ), pos );
   }

   inline void read_extension( pegtl_input_t& in, state& st )
   {
      const auto pos = in.position();

      do_inner_extension( in, st );

      if( st.temporary.is_string_type() ) {
         const std::string v = st.temporary.as< std::string >();
         const std::string f = read_file_throws( v );
         const std::byte* p = reinterpret_cast< const std::byte* >( f.data() );
         st.temporary.assign( std::vector< std::byte >( p, p + f.size() ), pos );
         return;
      }
      throw pegtl::parse_error( format( __FILE__, __LINE__, "require string as filename", { st.temporary.type() } ), pos );
   }

   inline void shell_extension( pegtl_input_t& in, state& st )
   {
      const auto pos = in.position();

      do_inner_extension( in, st );

      if( st.temporary.is_string_type() ) {
         const auto v = st.temporary.as< std::string >();
#if defined( _MSC_VER )
         throw pegtl::parse_error( format( __FILE__, __LINE__, "shell not supported on this platform", { { "command", v } } ), pos );
#else
         st.temporary.assign( shell_popen_throws( pos, st.temporary.as< std::string >() ), pos );

         if( !json::internal::validate_utf8_nothrow( v ) ) {
            throw pegtl::parse_error( format( __FILE__, __LINE__, "invalid utf-8 in shell output", { { "command", v } } ), pos );
         }
#endif
         return;
      }
      throw pegtl::parse_error( format( __FILE__, __LINE__, "require string for shell command", { st.temporary.type() } ), pos );
   }

   // clang-format off
   struct split_plus_ws : pegtl::plus< pegtl::space > {};
   struct split_star_ws : pegtl::star< pegtl::space > {};
   struct split_string : pegtl::plus< pegtl::not_one< ' ', '\n', '\r', '\t', '\v', '\f' > > {};
   struct split_grammar : pegtl::must< split_star_ws, pegtl::list_tail< split_string, split_plus_ws >, pegtl::eof > {};

   template< typename Rule > struct split_action : pegtl::nothing< Rule > {};
   // clang-format on

   template<>
   struct split_action< split_string >
   {
      template< typename Input >
      static void apply( const Input& in, json_t& temporary )
      {
         temporary.emplace_back( in.string(), in.position() );
      }
   };

   inline void split_extension( pegtl_input_t& in, state& st )
   {
      const auto pos = in.position();

      do_inner_extension( in, st );

      if( st.temporary.is_string_type() ) {
         pegtl::string_input< pegtl::tracking_mode::eager, pegtl_input_t::eol_t > i2( st.temporary.as< std::string >(), __FUNCTION__ );
         st.temporary.assign( json::empty_array, pos );
         pegtl::parse_nested< split_grammar, split_action >( in, i2, st.temporary );
         return;
      }
      throw pegtl::parse_error( format( __FILE__, __LINE__, "require string to split", { st.temporary.type() } ), pos );
   }

   inline std::string string_function( const tao::binary_view bv )
   {
      return std::string( reinterpret_cast< const char* >( bv.data() ), bv.size() );
   }

   inline json_t ubjson_function( const tao::binary_view bv )
   {
      return json::ubjson::basic_from_binary< value_traits >( bv );  // TODO: Positions.
   }

   inline void do_inner_extension( pegtl_input_t& in, state& st )
   {
      const auto pos = in.position();

      if( !in.empty() ) {
         switch( in.peek_char() ) {
            case '(':
               if( pegtl::parse< rules::inner, action, control >( in, st ) ) {
                  const auto ext = std::move( st.extension );
                  const auto& map = st.value_extensions;
                  const auto i = map.find( ext );

                  if( i != map.end() ) {
                     i->second( in, st );
                     pegtl::parse< pegtl::must< rules::round_z > >( in );
                     return;
                  }
                  throw pegtl::parse_error( format( __FILE__, __LINE__, "unknown value extension", { { "name", ext } } ), pos );
               }
               break;
            case '\'':
            case '"':
               st.temporary.assign( obtain_string( in ), pos );
               return;
            case '$':
               st.temporary.assign( obtain_binary( in ), pos );
               return;
            default:
               break;
         }
      }
      throw pegtl::parse_error( format( __FILE__, __LINE__, "invalid value extension", {} ), pos );
   }

}  // namespace tao::config::internal

#endif
