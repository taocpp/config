// Copyright (c) 2018-2024 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#include <algorithm>
#include <filesystem>
#include <iostream>
#include <stdexcept>

#include "test.hpp"

#include <tao/config.hpp>

namespace tao::config
{
   void setenv_throws( const std::string& name, const std::string& value )
   {
#if defined( _MSC_VER )
      const auto e = ::_putenv_s( name.c_str(), value.c_str() );
      if( e != 0 ) {
#else
      errno = 0;
      if( ::setenv( name.c_str(), value.c_str(), 1 ) != 0 ) {
         // LCOV_EXCL_START
         const auto e = errno;
#endif
         (void)e;
         throw std::runtime_error( "setenv failed" );
         // LCOV_EXCL_STOP
      }
   }

   template< template< typename... > class Traits >
   void unit_test( const std::filesystem::path& path )
   {
      try {
         const auto cc = basic_from_file< Traits >( path );
         const std::vector< std::filesystem::path > paths = { "tests/empty.success", path, "tests/empty.success" };
         const auto cf = basic_from_files< Traits >( paths );

         std::filesystem::path jaxn = path;
         jaxn.replace_extension( ".jaxn" );
         const auto cj = basic_from_file< Traits >( jaxn );
         const auto jj = json::jaxn::basic_from_file< Traits >( jaxn );

         const auto ccs = json::jaxn::to_string( cc );
         const auto cfs = json::jaxn::to_string( cf );
         const auto cjs = json::jaxn::to_string( cj );
         const auto jjs = json::jaxn::to_string( jj );

         if( ccs != cfs ) {
            // LCOV_EXCL_START
            ++failed;
            std::cerr << std::endl
                      << "Testcase '" << path << "' failed config test!" << std::endl;
            std::cerr << "<<< Config parsed as config <<<" << std::endl;
            std::cerr << ccs << std::endl;
            std::cerr << ">>> Config parsed as config >>>" << std::endl;
            std::cerr << "<<< Config parsed as config with empty files <<<" << std::endl;
            std::cerr << jjs << std::endl;
            std::cerr << ">>> Config parsed as config with empty files >>>" << std::endl;
            // LCOV_EXCL_STOP
         }
         if( ccs != jjs ) {
            // LCOV_EXCL_START
            ++failed;
            std::cerr << std::endl
                      << "Testcase '" << path << "' failed config test!" << std::endl;
            std::cerr << "<<< Config parsed as config <<<" << std::endl;
            std::cerr << ccs << std::endl;
            std::cerr << ">>> Config parsed as config >>>" << std::endl;
            std::cerr << "<<< Reference data parsed as jaxn <<<" << std::endl;
            std::cerr << jjs << std::endl;
            std::cerr << ">>> Reference data parsed as jaxn >>>" << std::endl;
            // LCOV_EXCL_STOP
         }
         if( ccs != cjs ) {
            // LCOV_EXCL_START
            ++failed;
            std::cerr << std::endl
                      << "Testcase '" << path << "' failed identity test!" << std::endl;
            std::cerr << "<<< Config parsed as config <<<" << std::endl;
            std::cerr << ccs << std::endl;
            std::cerr << ">>> Config parsed as config >>>" << std::endl;
            std::cerr << "<<< Reference data parsed as config <<<" << std::endl;
            std::cerr << cjs << std::endl;
            std::cerr << ">>> Reference data parsed as config >>>" << std::endl;
            // LCOV_EXCL_STOP
         }
         const auto fc = from_file( path );
         const auto ff = from_files( paths );

         const auto fcs = json::jaxn::to_string( fc );
         const auto ffs = json::jaxn::to_string( ff );

         if constexpr( std::is_same_v< decltype( cc ), decltype( fc ) > ) {
            TAO_CONFIG_TEST_ASSERT( ccs == fcs );
            TAO_CONFIG_TEST_ASSERT( cfs == ffs );
         }
         TAO_CONFIG_TEST_ASSERT( fcs == ffs );
      }
      // LCOV_EXCL_START
      catch( const std::exception& e ) {
         std::cerr << "Testcase '" << path << "' failed with exception '" << e.what() << "'" << std::endl;
         ++failed;
      }
      catch( const std::string& s ) {
         std::cerr << "Testcase '" << path << "' failed with error '" << s << "'" << std::endl;
         ++failed;
      }
      // LCOV_EXCL_STOP
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
         tao::config::setenv_throws( "TAO_CONFIG", "env_value" );
         tao::config::unit_test< tao::json::traits >( path );
         tao::config::unit_test< tao::config::traits >( path );
         ++count;
      }
   }
   if( tao::config::failed == 0 ) {
      std::cerr << "All " << count << " testcases passed." << std::endl;
   }
   return std::min( int( tao::config::failed ), 127 );
}
