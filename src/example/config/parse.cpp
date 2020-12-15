// Copyright (c) 2020 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#include <iostream>

#include <tao/config/internal/action.hpp>
#include <tao/config/internal/grammar.hpp>
#include <tao/config/internal/pegtl.hpp>
#include <tao/config/internal/state.hpp>
#include <tao/config/internal/to_stream.hpp>

namespace tao::config
{
   void test_parse_file( const std::filesystem::path& file )
   {
      internal::state st;
      std::cerr << file << std::endl;
      json::pegtl::file_input in( file );
      const bool result = internal::pegtl::parse< internal::rules::config_file, internal::action >( in, st );
      std::cerr << result << std::endl;
      internal::to_stream( std::cerr, st.root, 3 );
      std::cerr << std::endl;
   }

}  // tao::config

int main( int argc, char** argv )
{
   for( int i = 1; i < argc; ++i ) {
      tao::config::test_parse_file( argv[ i ] );
   }
   return 0;
}
