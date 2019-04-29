// Copyright (c) 2019 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#include <tao/config.hpp>
#include <tao/config/schema.hpp>

int main()
{
   const auto tcs = tao::config::schema::read( "tests/schema.tcs" );

   const auto data = tao::config::parse_file( "tests/schema.jaxn" );
   const auto result = tcs.validate( data );
   if( !result.is_uninitialized() ) {
      std::cerr << std::setw( 2 ) << result << std::endl;
   }

   return result ? 0 : 1;
}
