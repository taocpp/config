// Copyright (c) 2018-2024 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#include <filesystem>
#include <iostream>
#include <vector>

#include <tao/config.hpp>

#include <tao/json/jaxn/events/to_stream.hpp>

#include "try_catch.hpp"

int main( int argc, char** argv )
{
   tao::config::internal::try_catch( [ = ]() {
      const tao::config::value v = tao::config::from_files( std::vector< std::filesystem::path >( argv + 1, argv + argc ) );
      tao::json::jaxn::to_stream( std::cout, v, 3 );
      std::cout << std::endl;
   } );
   return 0;
}
