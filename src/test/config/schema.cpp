// Copyright (c) 2019-2020 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#include <tao/config.hpp>
#include <tao/config/schema.hpp>

int main()
{
   const auto tcs = tao::config::schema::from_file( "tests/schema.tcs" );
   const auto data = tao::config::from_file( "tests/schema.jaxn" );
   const auto error = tcs.validate( data );

   if( error ) {
      std::cerr << std::setw( 2 ) << error << std::endl;
   }
   return !error ? 0 : 1;
}
