// Copyright (c) 2018 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#include <iostream>

#include <tao/config/internal/parse_file.hpp>
#include <tao/config/internal/to_stream.hpp>

int main( int argc, char** argv )
{
   for( int i = 1; i < argc; ++i ) {
      tao::config::internal::state st;
      tao::config::internal::parse_file_impl( st, argv[ i ] );
      tao::config::internal::to_stream( std::cout, st.result, 3 );
      std::cout << std::endl;
   }
   return 0;
}
