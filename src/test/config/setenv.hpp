// Copyright (c) 2018-2024 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_SRC_TEST_CONFIG_SETENV_HPP
#define TAO_CONFIG_SRC_TEST_CONFIG_SETENV_HPP

#include <stdlib.h>
#include <stdexcept>
#include <string>

namespace tao::config::internal
{
   void setenv_throws( const std::string& name, const std::string& value )
   {
#if defined( _MSC_VER )
      const auto e = ::_putenv_s( name.c_str(), value.c_str() );
      if( e != 0 ) {
#else
      errno = 0;
      if( ::setenv( name.c_str(), value.c_str(), 1 ) != 0 ) {
         // LCOV_EXCL_START
         const auto e = errno;
#endif
         (void)e;
         throw std::runtime_error( "setenv failed" );
         // LCOV_EXCL_STOP
      }
   }

}  // namespace tao::config::internal

#endif
