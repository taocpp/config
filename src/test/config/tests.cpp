// Copyright (c) 2018-2019Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#include <tao/config.hpp>

namespace tao
{
   int analyse()
   {
      if( const auto p = tao::json_pegtl::analyze< tao::config::internal::rules::grammar >( true ) ) {
         std::cerr << "PEGTL grammar analysis found " << p << " problems!" << std::endl;
         return p;
      }
      return 0;
   }

   int failed = analyse();

   void unit_test( const std::string& name )
   {
      const auto cc = config::parse_file( name + ".config" );
      const auto cj = config::parse_file( name + "_only_data.jaxn" );
      const auto jj = json::jaxn::basic_parse_file< config::traits >( name + "_only_data.jaxn" );

      const auto ccs = json::jaxn::to_string( cc );
      const auto cjs = json::jaxn::to_string( cj );
      const auto jjs = json::jaxn::to_string( jj );

      if( ccs != jjs ) {
         ++failed;
         std::cerr << "Testcase '" << name << "' failed config test!" << std::endl;
         std::cerr << "<<< Config parsed as config <<<" << std::endl;
         std::cerr << ccs << std::endl;
         std::cerr << ">>> Config parsed as config >>>" << std::endl;
         std::cerr << "<<< Reference data parsed as jaxn <<<" << std::endl;
         std::cerr << jjs << std::endl;
         std::cerr << ">>> Reference data parsed as jaxn >>>" << std::endl;
      }
      if( ccs != cjs ) {
         ++failed;
         std::cerr << "Testcase '" << name << "' failed identity test!" << std::endl;
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
      tao::unit_test( argv[ i ] );
   }
   if( !tao::failed ) {
      std::cerr << "All testcases passed." << std::endl;
   }
   return tao::failed;
}
