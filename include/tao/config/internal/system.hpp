// Copyright (c) 2018-2019 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_INTERNAL_SYSTEM_HPP
#define TAO_CONFIG_INTERNAL_SYSTEM_HPP

#include <cstdlib>
#include <memory>
#include <optional>
#include <stdexcept>
#include <string>

#include <stdio.h>

#include "format.hpp"
#include "pegtl.hpp"

namespace tao::config::internal
{
   inline std::string read_file_throws( const std::string& f )
   {
      return pegtl::internal::file_reader( f.c_str() ).read();
   }

   inline std::optional< std::string > read_file_nothrow( const std::string& f )
   {
      try {
         return pegtl::internal::file_reader( f.c_str() ).read();
      }
      catch( const std::system_error& ) {
         return std::nullopt;
      }
   }

   inline std::string get_env_throws( const position& pos, const std::string& e )
   {
      if( const char* r = std::getenv( e.c_str() ) ) {
         return std::string( r );
      }
      throw pegtl::parse_error( format( "environment variable not found", { { "variable", e } } ), pos );
   }

   inline std::optional< std::string > get_env_nothrow( const std::string& e )
   {
      if( const char* r = std::getenv( e.c_str() ) ) {
         return std::string( r );
      }
      return std::nullopt;
   }

   inline std::string shell_popen_throws( const position& pos, const std::string& c )
   {
      errno = 0;

      const std::unique_ptr< FILE, void ( * )( FILE* ) > f( ::popen( c.c_str(), "r" ), []( FILE* f ) { ::pclose( f ); } );

      if( !f ) {
         throw pegtl::parse_error( format( "popen failed", { { "command", c }, { "errno", errno } } ), pos );
      }
      std::string r;
      char b[ 4096 ];

      while( const auto t = ::fread( b, 1, sizeof( b ), f.get() ) ) {
         r.append( b, t );
      }
      return r;
   }

}  // namespace tao::config::internal

#endif
