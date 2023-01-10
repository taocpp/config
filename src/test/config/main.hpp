// Copyright (c) 2014-2023 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_SRC_TEST_CONFIG_MAIN_HPP
#define TAO_CONFIG_SRC_TEST_CONFIG_MAIN_HPP

#include <cstdlib>
#include <iostream>

int main( int /*unused*/, char** argv )
{
   tao::config::unit_test();

   if( tao::config::failed != 0 ) {
      std::cerr << "config: unit test " << argv[ 0 ] << " failed " << tao::config::failed << std::endl;  // LCOV_EXCL_LINE
   }
   return ( tao::config::failed == 0 ) ? EXIT_SUCCESS : EXIT_FAILURE;
}

#endif
