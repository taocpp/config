// Copyright (c) 2018 Dr. Colin Hirsch and Daniel Frey
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

namespace tao
{
   namespace config
   {
      namespace internal
      {
         inline std::string read_file( const position&, const std::string& f )
         {
            return pegtl::internal::file_reader( f.c_str() ).read();
         }

         inline std::string get_env( const position& pos, const std::string& e )
         {
            if( const char* r = std::getenv( e.c_str() ) ) {
               return std::string( r );
            }
            throw std::runtime_error( format( "environment variable not found", { &pos, { "variable", e } } ) );
         }

         inline std::string shell_popen( const position& pos, const std::string& c )
         {
            errno = 0;

            const std::unique_ptr< FILE, void( * )( FILE* ) > f( ::popen( c.c_str(), "r" ), []( FILE* f ){ ::pclose( f ); } );

            if( !f ) {
               throw std::runtime_error( format( "popen failed", { &pos, { "command", c }, { "errno", errno } } ) );
            }
            std::string r;
            char b[ 4096 ];

            while( const auto t = ::fread( b, 1, sizeof( b ), f.get() ) ) {
               r.append( b, t );
            }
            return r;
         }

      }  // namespace internal

   }  // namespace config

}  // namespace tao

#endif
