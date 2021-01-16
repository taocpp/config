// Copyright (c) 2020-2021 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#include <iostream>
#include <stdexcept>
#include <string>

#include <tao/config.hpp>

#include <tao/config/internal/to_stream.hpp>

int main( int argc, char** argv )
{
   tao::config::internal::config_parser cfg;

   try {
      for( int i = 1; i < argc; ++i ) {
         const std::filesystem::path file( argv[ i ] );
         std::cout << "PARSE " << file << std::endl;
         cfg.parse( file );
         tao::config::internal::to_stream( std::cout, cfg.st.root, 3 );
         std::cout << std::endl;
      }
      std::cout << "PHASE 2" << std::endl;
      cfg.phase2_loop();
      tao::config::internal::to_stream( std::cout, cfg.st.root, 3 );
      std::cout << std::endl;
      std::cout << "PHASE 3" << std::endl;
      tao::config::internal::phase3_remove( cfg.st.root );
      tao::config::internal::to_stream( std::cout, cfg.st.root, 3 );
      std::cout << std::endl;
      std::cout << "PHASE 4" << std::endl;
      tao::config::internal::phase4_schema( cfg.st.root, cfg.st.schema );
      std::cout << "   Schema validation does not change anything." << std::endl;
      std::cout << "RESULT" << std::endl;
      const tao::config::value j = tao::config::internal::phase5_repack< tao::config::traits >( cfg.st.root );
      tao::config::to_stream( std::cout, j, 3 );
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
