// Copyright (c) 2018-2019 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#include <tao/config.hpp>

#include <tao/config/internal/try_catch.hpp>

int main( int argc, char** argv )
{
   tao::config::internal::try_catch( [=](){
      tao::config::internal::state st;
      for( int i = 1; i < argc; ++i ) {
         tao::json::pegtl::file_input in( argv[ i ] );
         tao::json::pegtl::parse< tao::config::internal::rules::grammar, tao::config::internal::action, tao::config::internal::control >( in, st );
      }
      tao::config::internal::to_stream( std::cout, st.root, 3 );
      std::cout << std::endl;
   } );
   return 0;
}
