// Copyright (c) 2019 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#include <tao/config.hpp>
#include <tao/config/schema.hpp>

int main()
{
   const auto tcs = tao::config::schema::read( "tests/schema.tcs" );

   const auto data = tao::json::jaxn::basic_parse_file< tao::config::traits >( "tests/schema.jaxn" );
   assert( tcs.validate( data ) );

   return 0;
}
