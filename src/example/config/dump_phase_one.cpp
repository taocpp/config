// Copyright (c) 2018 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#include <iostream>

#include <tao/config.hpp>

int main( int argc, char** argv )
{
   for( int i = 1; i < argc; ++i ) {
      tao::config::internal::state st;
      tao::json_pegtl::file_input in( argv[ i ] );
      tao::json_pegtl::parse< tao::config::internal::rules::grammar, tao::config::internal::action, tao::config::internal::control >( in, st );
      tao::config::internal::to_stream( std::cout, st.result, 3 );
      std::cout << std::endl;
   }
   return 0;
}
