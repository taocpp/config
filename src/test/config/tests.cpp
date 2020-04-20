// Copyright (c) 2018-2020 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#include <algorithm>

#include <tao/config.hpp>

namespace tao
{
   int analyse()
   {
      // TODO!
      // if( const auto p = tao::json::pegtl::analyze< tao::config::internal::rules::config_file >( true ) ) {
      //    std::cerr << std::endl
      //              << "PEGTL grammar analysis found " << p << " problems!" << std::endl;
      //    return p;
      // }
      return 0;
   }

   int failed = analyse();

   template< template< typename... > class Traits >
   void unit_test( const std::string& name )
   {
      const auto cc = config::basic_from_file< Traits >( name + ".config" );
      const auto cj = config::basic_from_file< Traits >( name + "_only_data.jaxn" );
      const auto jj = json::jaxn::basic_from_file< Traits >( name + "_only_data.jaxn" );

      const auto ccs = json::jaxn::to_string( cc );
      const auto cjs = json::jaxn::to_string( cj );
      const auto jjs = json::jaxn::to_string( jj );

      if( ccs != jjs ) {
         ++failed;
         std::cerr << std::endl
                   << "Testcase '" << name << "' failed config test!" << std::endl;
         std::cerr << "<<< Config parsed as config <<<" << std::endl;
         std::cerr << ccs << std::endl;
         std::cerr << ">>> Config parsed as config >>>" << std::endl;
         std::cerr << "<<< Reference data parsed as jaxn <<<" << std::endl;
         std::cerr << jjs << std::endl;
         std::cerr << ">>> Reference data parsed as jaxn >>>" << std::endl;
      }
      if( ccs != cjs ) {
         ++failed;
         std::cerr << std::endl
                   << "Testcase '" << name << "' failed identity test!" << std::endl;
         std::cerr << "<<< Config parsed as config <<<" << std::endl;
         std::cerr << ccs << std::endl;
         std::cerr << ">>> Config parsed as config >>>" << std::endl;
         std::cerr << "<<< Reference data parsed as config <<<" << std::endl;
         std::cerr << cjs << std::endl;
         std::cerr << ">>> Reference data parsed as config >>>" << std::endl;
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
