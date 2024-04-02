// Copyright (c) 2018-2024 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_INTERNAL_SYSTEM_UTILITY_HPP
#define TAO_CONFIG_INTERNAL_SYSTEM_UTILITY_HPP

#include <stdio.h>

#include <cstdlib>
#include <memory>
#include <optional>
#include <stdexcept>
#include <string>
#include <utility>

#include "pegtl.hpp"

namespace tao::config::internal
{
   // TODO: Use std::filesystem::path for filenames?

   [[nodiscard]] inline std::string read_file_throws( const std::string& filename )
   {
      return pegtl::internal::read_file_stdio( filename ).read_string();
   }

   [[nodiscard]] inline std::optional< std::string > read_file_nothrow( const std::string& filename )
   {
      try {
         return read_file_throws( filename );
      }
      catch( const std::system_error& ) {
         return std::nullopt;
      }
   }

   [[nodiscard]] inline std::optional< std::string > getenv_nothrow( const std::string& name )
   {
#if defined( _MSC_VER )
      char buffer[ 256 ];
      std::size_t s = 0;
      if( ::getenv_s( &s, buffer, name.c_str() ) == 0 ) {
         if( s > 0 ) {
            return std::string( buffer );
         }
      }
      // TODO: Check s and try with a larger buffer
#else
      if( const char* r = std::getenv( name.c_str() ) ) {
         return std::string( r );
      }
#endif
      return std::nullopt;
   }

   [[nodiscard]] inline std::string getenv_throws( const pegtl::position& pos, const std::string& name )
   {
      if( auto result = getenv_nothrow( name ) ) {
         return std::move( *result );
      }
      throw pegtl::parse_error( "environment variable '" + name + "' not found", pos );
   }

#if !defined( _MSC_VER )
   [[nodiscard]] inline std::string shell_popen_throws( const pegtl::position& pos, const std::string& script )
   {
      errno = 0;

      std::unique_ptr< FILE, void ( * )( FILE* ) > file( ::popen( script.c_str(), "r" ), []( FILE* f ) { ::pclose( f ); } );

      if( !file ) {
         throw pegtl::parse_error( "TODO: Better error message for shell function error.", pos );
         //         throw pegtl::parse_error( format( __FILE__, __LINE__, "popen failed", { { "command", script }, { "errno", errno } } ), pos );
      }
      std::string result;
      char buffer[ 1000 ];

      while( const auto temp = ::fread( buffer, 1, sizeof( buffer ), file.get() ) ) {
         result.append( buffer, temp );
      }
      errno = 0;

      if( ( errno != 0 ) || ( ::pclose( file.release() ) != 0 ) ) {
         throw pegtl::parse_error( "TODO: Better error message for shell function error.", pos );
      }
      return result;
   }
#endif

}  // namespace tao::config::internal

#endif
