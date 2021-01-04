// Copyright (c) 2020 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#include <iostream>
#include <stdexcept>
#include <string>

#include <tao/config/internal/config_parser.hpp>
#include <tao/config/internal/to_stream.hpp>

int main( int argc, char** argv )
{
   tao::config::internal::config_parser cfg;

   try {
      for( int i = 1; i < argc; ++i ) {
         const std::filesystem::path file( argv[ i ] );
         std::cout << "PARSE " << file << std::endl;
         cfg.parse( file );
      }
      std::cout << "PHASE 2" << std::endl;
      cfg.phase2();
      tao::config::internal::to_stream( std::cout, cfg.st.root, 3 );
      std::cout << std::endl;
   }
   catch( const std::exception& e ) {
      std::cerr << "ERROR " << e.what() << std::endl;
      return 1;
   }
   catch( const std::string& e ) {
      std::cerr << "STRING " << e << std::endl;
      return 1;
   }
   return 0;
}
