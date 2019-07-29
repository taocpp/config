// Copyright (c) 2018-2019 Dr. Colin Hirsch and Daniel Frey
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

   inline void binary_extension( pegtl_input_t& in, state& st )
   {
      const auto pos = in.position();

      do_inner_extension( in, st );

      if( st.temporary.is_string_type() ) {
         const auto sv = st.temporary.as< std::string_view >();
         const auto* const sp = reinterpret_cast< const std::byte* >( sv.data() );
         st.temporary.assign( std::vector< std::byte >( sp, sp + sv.size() ), pos );
         return;
      }
      throw pegtl::parse_error( format( __FILE__, __LINE__, "require string to convert to binary", { st.temporary.type() } ), pos );
   }

   inline void cbor_extension( pegtl_input_t& in, state& st )
   {
      const auto pos = in.position();

      do_inner_extension( in, st );

      if( st.temporary.is_string_type() ) {
         st.temporary = json::cbor::basic_from_string< value_traits >( st.temporary.as< std::string >() );
         return;
      }
      throw pegtl::parse_error( format( __FILE__, __LINE__, "require string to parse cbor", { st.temporary.type() } ), pos );
   }

   inline void env_extension( pegtl_input_t& in, state& st )
   {
      const auto pos = in.position();
      const auto v = obtain_string( in );

      st.temporary.assign( get_env_throws( pos, v ), pos );

      if( !json::internal::validate_utf8_nothrow( st.temporary.as< std::string >() ) ) {
         throw pegtl::parse_error( format( __FILE__, __LINE__, "invalid utf-8 in environment variable", { { "name", v } } ), pos );
      }
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

   inline void jaxn_extension( pegtl_input_t& in, state& st )
   {
      const auto pos = in.position();

      do_inner_extension( in, st );

      if( st.temporary.is_string_type() ) {
         st.temporary = json::jaxn::basic_from_string< value_traits >( st.temporary.as< std::string >() );
         return;
      }
      throw pegtl::parse_error( format( __FILE__, __LINE__, "require string to parse jaxn", { st.temporary.type() } ), pos );
   }

   inline void json_extension( pegtl_input_t& in, state& st )
   {
      const auto pos = in.position();

      do_inner_extension( in, st );

      if( st.temporary.is_string_type() ) {
         st.temporary = json::basic_from_string< value_traits >( st.temporary.as< std::string >() );
         return;
      }
      throw pegtl::parse_error( format( __FILE__, __LINE__, "require string to parse json", { st.temporary.type() } ), pos );
   }

   inline void msgpack_extension( pegtl_input_t& in, state& st )
   {
      const auto pos = in.position();

      do_inner_extension( in, st );

      if( st.temporary.is_string_type() ) {
         st.temporary = json::msgpack::basic_from_string< value_traits >( st.temporary.as< std::string >() );
         return;
      }
      throw pegtl::parse_error( format( __FILE__, __LINE__, "require string to parse msgpack", { st.temporary.type() } ), pos );
   }

   inline void parse_extension( pegtl_input_t& in, state& st )
   {
      assert( !st.lstack.empty() );

      const auto pos = in.position();

      do_inner_extension( in, st );

      if( st.temporary.is_string_type() ) {
         pegtl::string_input< pegtl::tracking_mode::eager, pegtl_input_t::eol_t, const char* > i2( st.temporary.as< std::string >(), __FUNCTION__ );
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
         const auto v = st.temporary.as< std::string >();

         st.temporary.assign( read_file_throws( v ), pos );

         if( !json::internal::validate_utf8_nothrow( st.temporary.as< std::string >() ) ) {
            throw pegtl::parse_error( format( __FILE__, __LINE__, "invalid utf-8 in file", { { "filename", v } } ), pos );
         }
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

         st.temporary.assign( shell_popen_throws( pos, st.temporary.as< std::string >() ), pos );

         if( !json::internal::validate_utf8_nothrow( v ) ) {
            throw pegtl::parse_error( format( __FILE__, __LINE__, "invalid utf-8 in shell output", { { "command", v } } ), pos );
         }
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
         pegtl::string_input< pegtl::tracking_mode::eager, pegtl_input_t::eol_t, const char* > i2( st.temporary.as< std::string >(), __FUNCTION__ );
         st.temporary.assign( json::empty_array, pos );
         pegtl::parse_nested< split_grammar, split_action >( in, i2, st.temporary );
         return;
      }
      throw pegtl::parse_error( format( __FILE__, __LINE__, "require string to split", { st.temporary.type() } ), pos );
   }

   inline void string_extension( pegtl_input_t& in, state& st )
   {
      const auto pos = in.position();

      do_inner_extension( in, st );

      if( st.temporary.is_binary_type() ) {
         const auto bv = st.temporary.as< binary_view >();
         std::string v( reinterpret_cast< const char* >( bv.data() ), bv.size() );

         if( !json::internal::validate_utf8_nothrow( v ) ) {
            throw pegtl::parse_error( format( __FILE__, __LINE__, "invalid utf-8 in binary data", { { "command", v } } ), pos );
         }
         st.temporary.assign( std::move( v ), pos );
         return;
      }
      throw pegtl::parse_error( format( __FILE__, __LINE__, "require binary to convert to string", { st.temporary.type() } ), pos );
   }

   inline void ubjson_extension( pegtl_input_t& in, state& st )
   {
      const auto pos = in.position();

      do_inner_extension( in, st );

      if( st.temporary.is_string_type() ) {
         st.temporary = json::ubjson::basic_from_string< value_traits >( st.temporary.as< std::string >() );
         return;
      }
      throw pegtl::parse_error( format( __FILE__, __LINE__, "require string to parse ubjson", { st.temporary.type() } ), pos );
   }

   inline void do_inner_extension( pegtl_input_t& in, state& st )
   {
      const auto pos = in.position();

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
      st.temporary.assign( obtain_string( in ), pos );
   }

}  // namespace tao::config::internal

#endif
