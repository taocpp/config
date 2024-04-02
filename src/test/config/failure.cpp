// Copyright (c) 2020-2024 Dr. Colin Hirsch and Daniel Frey
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
   unsigned failed = 0;

   template< template< typename... > class Traits >
   void unit_test( const std::filesystem::path& path )
   {
      try {
         // For a failure testcase to succeed the next line must throw an error.
         const auto cc = config::basic_from_file< Traits >( path );
         // LCOV_EXCL_START
         const auto ccs = json::jaxn::to_string( cc );
         ++failed;
         std::cerr << std::endl
                   << "Testcase '" << path << "' failed error test!" << std::endl;
         std::cerr << "<<< Config parsed as config <<<" << std::endl;
         std::cerr << ccs << std::endl;
         std::cerr << ">>> Config parsed as config >>>" << std::endl;
         // LCOV_EXCL_STOP
      }
      catch( const pegtl::parse_error_base& e ) {
         std::cout << ansi_text << "pegtl::parse_error: " << ansi_message << e.message() << ": " << ansi_source << e.position_string() << ansi_reset << std::endl;
      }
      catch( const std::exception& e ) {
         std::cout << "std::exception: " << e.what() << std::endl;
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
   if( tao::failed == 0 ) {
      std::cerr << "All " << count << " testcases passed." << std::endl;
   }
   return std::min( int( tao::failed ), 127 );
}
