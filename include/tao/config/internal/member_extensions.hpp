// Copyright (c) 2018-2020 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_INTERNAL_MEMBER_EXTENSIONS_HPP
#define TAO_CONFIG_INTERNAL_MEMBER_EXTENSIONS_HPP

#include <cerrno>
#include <system_error>
#include <utility>

#include "extension_t.hpp"
#include "extension_utility.hpp"
#include "format.hpp"
#include "inner_extensions.hpp"
#include "phase1_access.hpp"
#include "phase1_erase.hpp"
#include "system.hpp"

namespace tao::config::internal
{
   inline void erase_extension( pegtl_input_t& in, state& st )
   {
      assert( !st.ostack.empty() );

      const auto pos = in.position();
      const auto k = obtain_key( in, st );

      if( phase1_erase( pos, *st.ostack.back(), k ) == 0 ) {
         throw pegtl::parse_error( format( __FILE__, __LINE__, "nothing to delete", { &k } ), pos );
      }
   }

   inline void erase_if_extension( pegtl_input_t& in, state& st )
   {
      assert( !st.ostack.empty() );

      const auto pos = in.position();
      const auto p = obtain_key( in, st );

      phase1_erase( pos, *st.ostack.back(), p );
   }

   inline void include_extension( pegtl_input_t& in, state& st )
   {
      auto pos = in.position();

      do_inner_extension( in, st );

      if( !st.temporary.is_string_type() ) {
         throw pegtl::parse_error( format( __FILE__, __LINE__, "include requires string", { st.temporary.type() } ), pos );
      }
      const auto f = st.temporary.as< std::string >();

      try {
         pegtl::file_input i2( f );
         pegtl::parse< rules::config_file, action, control >( static_cast< pegtl_input_t& >( i2 ), st );
         st.temporary.set_uninitialized();
      }
      catch( const std::system_error& e ) {
         throw pegtl::parse_error( format( __FILE__, __LINE__, "include failed", { { "filename", f }, { "error", e.what() }, { "errno", e.code().value() } } ), pos );
      }
      catch( pegtl::parse_error& e ) {
         e.add_position( std::move( pos ) );
         throw;
      }
   }

   inline void include_if_extension( pegtl_input_t& in, state& st )
   {
      auto pos = in.position();

      do_inner_extension( in, st );

      if( st.temporary.is_null() ) {
         return;
      }
      if( !st.temporary.is_string_type() ) {
         throw pegtl::parse_error( format( __FILE__, __LINE__, "include requires string", { st.temporary.type() } ), pos );
      }
      const auto f = st.temporary.as< std::string >();

      try {
         pegtl::file_input i2( f );
         pegtl::parse< rules::config_file, action, control >( static_cast< pegtl_input_t& >( i2 ), st );
         st.temporary.set_uninitialized();
      }
      catch( const std::system_error& e ) {
         if( e.code().value() != ENOENT ) {
            throw pegtl::parse_error( format( __FILE__, __LINE__, "include? failed", { { "filename", f }, { "error", e.what() }, { "errno", e.code().value() } } ), pos );
         }
      }
      catch( pegtl::parse_error& e ) {
         e.add_position( std::move( pos ) );
         throw;
      }
   }

   inline void schema_extension( pegtl_input_t& in, state& st )
   {
      const auto pos = in.position();

      do_inner_extension( in, st );

      if( st.temporary.is_null() ) {
         st.schema.clear();
         return;
      }
      if( st.temporary.is_string_type() ) {
         st.schema = st.temporary.as< std::string >();
         return;
      }
      throw pegtl::parse_error( format( __FILE__, __LINE__, "schema requires string or null", { st.temporary.type() } ), pos );
   }

   inline void setenv_extension( pegtl_input_t& in, state& )
   {
      const auto pos = in.position();

      const auto n = obtain_string( in );
      pegtl::parse< pegtl::must< rules::wsp > >( in );
      const auto v = obtain_string( in );

#if defined( _MSC_VER )
      throw pegtl::parse_error( format( __FILE__, __LINE__, "setenv not supported on this platform", { { "name", n }, { "value", v } } ), pos );
#else
      set_env_throws( pos, n, v );
#endif
   }

   inline void stderr_extension( pegtl_input_t& in, state& st )
   {
      assert( !st.ostack.empty() );

      const auto pos = in.position();
      const auto p = obtain_key( in, st );

      to_stream( std::cerr, phase1_access( pos, *st.ostack.back(), p ), 3 );
      std::cerr << std::endl;
   }

   inline void temporary_compute_current_prefix( const entry& haystack, const entry* needle, const key& prefix );

   inline void temporary_compute_current_prefix( const concat& haystack, const entry* needle, const key& prefix )
   {
      for( const auto& e : haystack.entries() ) {
         temporary_compute_current_prefix( e, needle, prefix );
      }
   }

   inline void temporary_compute_current_prefix( const entry& haystack, const entry* needle, const key& prefix )
   {
      if( &haystack == needle ) {
         throw prefix;
      }
      switch( haystack.type() ) {
         case entry::atom:
         case entry::nothing:
         case entry::reference:
            return;
         case entry::array: {
            std::size_t c = 0;
            for( const auto& i : haystack.get_array().list() ) {
               temporary_compute_current_prefix( i, needle, prefix + c++ );
            }
         } break;
         case entry::object:
            for( const auto& i : haystack.get_object().map() ) {
               temporary_compute_current_prefix( i.second, needle, prefix + i.first );
            }
            break;
      }
   }

   inline key temporary_compute_current_prefix( state& st )
   {
      try {
         temporary_compute_current_prefix( st.root, st.ostack.back(), key() );
      }
      catch( const key& k ) {
         return k;
      }
      assert( false );
   }

   inline void temporary_extension( pegtl_input_t& in, state& st )
   {
      assert( !st.ostack.empty() );

      const auto pos = in.position();
      const auto p = obtain_key( in, st );

      st.temporaries.emplace_back( temporary_compute_current_prefix( st ) + p );
   }

   inline bool do_member_extension( pegtl_input_t& in, state& st )
   {
      const auto pos = in.position();
      pegtl::parse< rules::outer, action, control >( in, st );

      const auto ext = std::move( st.extension );
      const auto& map = st.member_extensions;
      const auto i = map.find( ext );

      if( i != map.end() ) {
         i->second( in, st );
         return true;
      }
      throw pegtl::parse_error( format( __FILE__, __LINE__, "unknown member extension", { { "name", ext } } ), pos );
   }

}  // namespace tao::config::internal

#endif
