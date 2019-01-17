// Copyright (c) 2018-2019Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#include <tao/config.hpp>

int main( int argc, char** argv )
{
   const tao::config::value v = tao::config::parse_files( std::vector< std::string >( argv + 1, argv + argc ) );
   tao::config::to_stream( std::cout, v, 3 );
   std::cout << std::endl;
   return 0;
}
