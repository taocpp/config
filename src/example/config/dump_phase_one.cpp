// Copyright (c) 2018-2020 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#include <tao/config.hpp>

#include <tao/config/internal/try_catch.hpp>

int main( int argc, char** argv )
{
   tao::config::internal::try_catch( [=]() {
      tao::config::internal::configurator c;
      for( int i = 1; i < argc; ++i ) {
         c.parse( argv[ i ] );
      }
      tao::config::internal::to_stream( std::cout, c.st.root, 3 );
      std::cout << std::endl;
   } );
   return 0;
}
