// Copyright (c) 2020 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#include <iostream>
#include <stdexcept>
#include <string>

#include <tao/config/internal/config_action.hpp>
#include <tao/config/internal/config_grammar.hpp>
#include <tao/config/internal/config_parser.hpp>
#include <tao/config/internal/pegtl.hpp>
#include <tao/config/internal/to_stream.hpp>

namespace tao::config
{
   void test_parse_file( const std::filesystem::path& file )
   {
      internal::config_parser cfg;
      std::cerr << file << std::endl;
      json::pegtl::file_input in( file );
      const bool result = internal::pegtl::parse< internal::rules::config_file, internal::config_action >( in, cfg.st, cfg.em );
      std::cerr << result << std::endl;
      internal::to_stream( std::cerr, cfg.st.root, 3 );
      std::cerr << std::endl;
   }

}  // namespace tao::config

int main( int argc, char** argv )
{
   for( int i = 1; i < argc; ++i ) {
      try {
         tao::config::test_parse_file( argv[ i ] );
      }
      catch( const std::exception& e ) {
         std::cerr << "ERROR " << e.what() << std::endl;
      }
      catch( const std::string& e ) {
         std::cerr << "STRING " << e << std::endl;
      }
   }
   return 0;
}
