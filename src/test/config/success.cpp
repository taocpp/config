// Copyright (c) 2018-2022 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#include <algorithm>
#include <filesystem>
#include <iostream>

#include <tao/config.hpp>

namespace tao
{
   int failed = 0;

   template< template< typename... > class Traits >
   void unit_test( const std::filesystem::path& path )
   {
      try {
         const auto cc = config::basic_from_file< Traits >( path );
         std::filesystem::path jaxn = path;
         jaxn.replace_extension( ".jaxn" );
         const auto cj = config::basic_from_file< Traits >( jaxn );
         const auto jj = json::jaxn::basic_from_file< Traits >( jaxn );

         const auto ccs = json::jaxn::to_string( cc );
         const auto cjs = json::jaxn::to_string( cj );
         const auto jjs = json::jaxn::to_string( jj );

         if( ccs != jjs ) {
            ++failed;
            std::cerr << std::endl
                      << "Testcase '" << path << "' failed config test!" << std::endl;
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
                      << "Testcase '" << path << "' failed identity test!" << std::endl;
            std::cerr << "<<< Config parsed as config <<<" << std::endl;
            std::cerr << ccs << std::endl;
            std::cerr << ">>> Config parsed as config >>>" << std::endl;
            std::cerr << "<<< Reference data parsed as config <<<" << std::endl;
            std::cerr << cjs << std::endl;
            std::cerr << ">>> Reference data parsed as config >>>" << std::endl;
         }
      }
      catch( const std::exception& e ) {
         std::cerr << "Testcase '" << path << "' failed with exception '" << e.what() << "'" << std::endl;
         ++failed;
      }
      catch( const std::string& s ) {
         std::cerr << "Testcase '" << path << "' failed with error '" << s << "'" << std::endl;
         ++failed;
      }
   }

}  // namespace tao

int main()
{
   unsigned count = 0;

   for( const auto& entry : std::filesystem::directory_iterator( "tests" ) ) {
      if( const auto& path = entry.path(); path.extension() == ".success" ) {
#if defined( _MSC_VER )
         if( entry.path().stem() == "shell" ) {
            continue;
         }
#endif
         tao::unit_test< tao::json::traits >( path );
         tao::unit_test< tao::config::traits >( path );
         ++count;
      }
   }
   if( !tao::failed ) {
      std::cerr << "All " << count << " testcases passed." << std::endl;
   }
   return std::min( tao::failed, 127 );
}
