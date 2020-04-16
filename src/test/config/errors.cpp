// Copyright (c) 2020 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#include <algorithm>

#include <tao/config.hpp>

namespace tao
{
   int failed = 0;

   template< template< typename... > class Traits >
   void unit_test( const std::string& name )
   {
      try {
         const auto cc = config::basic_from_file< Traits >( name + ".error" );
         const auto ccs = json::jaxn::to_string( cc );
         ++failed;
         std::cerr << std::endl
                   << "Testcase '" << name << "' failed error test!" << std::endl;
         std::cerr << "<<< Config parsed as config <<<" << std::endl;
         std::cerr << ccs << std::endl;
         std::cerr << ">>> Config parsed as config >>>" << std::endl;
      }
      catch( const std::exception& ) {
      }
   }

}  // namespace tao

int main( int argc, char** argv )
{
   for( int i = 1; i < argc; ++i ) {
      tao::unit_test< tao::json::traits >( argv[ i ] );
      tao::unit_test< tao::config::traits >( argv[ i ] );
   }
   if( !tao::failed ) {
      std::cerr << "All testcases passed." << std::endl;
   }
   return std::min( tao::failed, 127 );
}
