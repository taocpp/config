// Copyright (c) 2018-2021 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_INTERNAL_SYSTEM_UTILITY_HPP
#define TAO_CONFIG_INTERNAL_SYSTEM_UTILITY_HPP

#include <stdio.h>

#include <cstdlib>
#include <memory>
#include <optional>
#include <stdexcept>
#include <string>

#include "pegtl.hpp"

namespace tao::config::internal
{
   // TODO: Use std::filesystem::path for filenames?

   [[nodiscard]] inline std::string read_file_throws( const std::string& filename )
   {
      return pegtl::internal::file_reader( filename ).read();
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

   [[nodiscard]] inline std::string get_env_throws( const pegtl::position& pos, const std::string& name )
   {
      if( const char* r = std::getenv( name.c_str() ) ) {
         return std::string( r );
      }
      throw pegtl::parse_error( "environment variable not found " + name, pos );
   }

   [[nodiscard]] inline std::optional< std::string > get_env_nothrow( const std::string& name )
   {
      if( const char* r = std::getenv( name.c_str() ) ) {
         return std::string( r );
      }
      return std::nullopt;
   }

#if !defined( _MSC_VER )
   inline void set_env_throws( const pegtl::position& pos, const std::string& name, const std::string& value )
   {
      errno = 0;
      if( ::setenv( name.c_str(), value.c_str(), 1 ) != 0 ) {
         //         const auto e = errno;
         throw std::string( "TODO" );
         //         throw pegtl::parse_error( format( __FILE__, __LINE__, "failed to set environment variable", { { "name", name }, { "value", value }, { "errno", e } } ), pos );
      }
   }

   [[nodiscard]] inline std::string shell_popen_throws( const pegtl::position& pos, const std::string& script )
   {
      errno = 0;

      const std::unique_ptr< FILE, void ( * )( FILE* ) > file( ::popen( script.c_str(), "r" ), []( FILE* f ) { ::pclose( f ); } );

      if( !file ) {
         throw std::string( "TODO" );
         //         throw pegtl::parse_error( format( __FILE__, __LINE__, "popen failed", { { "command", script }, { "errno", errno } } ), pos );
      }
      std::string result;
      char buffer[ 1000 ];

      while( const auto temp = ::fread( buffer, 1, sizeof( buffer ), file.get() ) ) {
         result.append( buffer, temp );
      }
      return result;
   }
#endif

}  // namespace tao::config::internal

#endif
