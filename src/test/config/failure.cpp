// Copyright (c) 2020-2021 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#include <algorithm>
#include <filesystem>
#include <iostream>

#include <tao/config.hpp>

const char* ansi_reset = "\033[0m";
const char* ansi_message = "\033[1;31m";
const char* ansi_source = "\033[36m";
const char* ansi_text = "\033[33m";

namespace tao
{
   int failed = 0;

   template< template< typename... > class Traits >
   void unit_test( const std::filesystem::path& path )
   {
      try {
         const auto cc = config::basic_from_file< Traits >( path );
         const auto ccs = json::jaxn::to_string( cc );
         ++failed;
         std::cerr << std::endl
                   << "Testcase '" << path << "' failed error test!" << std::endl;
         std::cerr << "<<< Config parsed as config <<<" << std::endl;
         std::cerr << ccs << std::endl;
         std::cerr << ">>> Config parsed as config >>>" << std::endl;
      }
      catch( const pegtl::parse_error& e ) {
         const auto& pos = e.positions();
         auto it = pos.begin();
         std::cout << ansi_text << "pegtl::parse_error: " << ansi_source << *it << ": " << ansi_message << e.message() << ansi_reset << std::endl;
         while( ++it != pos.end() ) {
            std::cout << ansi_text << "  from: " << ansi_source << *it << ansi_reset << std::endl;
         }
      }
      catch( const std::exception& e ) {
         std::cout << "std::exception: " << e.what() << std::endl;
      }
      catch( const std::string& s ) {
         std::cout << "Exception with std::string: " << s << std::endl;
         // TODO: Remove catch string when all temporary throw strings have been replaced with proper exceptions.
      }
   }

}  // namespace tao

int main()
{
   unsigned count = 0;

   for( const auto& entry : std::filesystem::directory_iterator( "tests" ) ) {
      if( const auto& path = entry.path(); path.extension() == ".failure" ) {
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
