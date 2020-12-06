// Copyright (c) 2020 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#include <algorithm>
#include <filesystem>

#include <tao/config.hpp>

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
      catch( const std::exception& ) {
      }
   }

}  // namespace tao

int main()
{
   unsigned count = 0;

   for( const std::filesystem::directory_entry& entry : std::filesystem::directory_iterator( "tests" ) ) {
      if( const std::filesystem::path& path = entry.path(); path.extension() == ".failure" ) {
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
