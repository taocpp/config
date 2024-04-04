// Copyright (c) 2024 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#include <algorithm>
#include <filesystem>
#include <iostream>
#include <set>
#include <stdexcept>
#include <vector>

#include "setenv.hpp"
#include "test.hpp"

#include <tao/config.hpp>

namespace tao::config
{
   std::size_t iteration = 0;

   [[nodiscard]] std::size_t p2fun( internal::state& st, const internal::function_map& fm )
   {
      return internal::phase2_functions( st, fm );
   }

   [[nodiscard]] std::size_t p2add( internal::state& st, const internal::function_map& /*unused*/ )
   {
      return internal::phase2_additions( st.root );
   }

   [[nodiscard]] std::size_t p2ref( internal::state& st, const internal::function_map& /*unused*/ )
   {
      return internal::phase2_references( st.root );
   }

   [[nodiscard]] std::size_t p2ast( internal::state& st, const internal::function_map& /*unused*/ )
   {
      return internal::phase2_asterisks( st.root );
   }

   using func_t = std::size_t( * )( internal::state&, const internal::function_map& );

   std::set< func_t > p2funcs = { p2fun, p2add, p2ref, p2ast };

   void unit_test( const std::filesystem::path& path )
   {
      try {
         std::filesystem::path jaxn = path;
         jaxn.replace_extension( ".jaxn" );
         const auto cj = from_file( jaxn );
         const auto cjs = json::jaxn::to_string( cj );

         std::vector< func_t > v( p2funcs.begin(), p2funcs.end() );

         do {
            internal::config_parser p;
            p.parse( path );

            while( v[ 0 ]( p.st, p.fm ) | v[ 1 ]( p.st, p.fm ) | v[ 2 ]( p.st, p.fm ) | v[ 3 ]( p.st, p.fm ) );

            internal::phase3_remove( p.st.root );
            const auto cc = internal::phase5_repack< traits >( p.st.root );
            const auto ccs = json::jaxn::to_string( cc );

            if( ccs != cjs ) {
               // LCOV_EXCL_START
               ++failed;
               std::cerr << std::endl
                         << "Testcase '" << path << "' failed identity test in iteration " << iteration << std::endl;
               std::cerr << "<<< Config parsed as config <<<" << std::endl;
               std::cerr << ccs << std::endl;
               std::cerr << ">>> Config parsed as config >>>" << std::endl;
               std::cerr << "<<< Reference data parsed as config <<<" << std::endl;
               std::cerr << cjs << std::endl;
               std::cerr << ">>> Reference data parsed as config >>>" << std::endl;
               // LCOV_EXCL_STOP
            }
            ++iteration;
         } while( std::next_permutation( v.begin(), v.end() ) );
      }
      // LCOV_EXCL_START
      catch( const std::exception& e ) {
         std::cerr << "Testcase '" << path << "' failed with exception '" << e.what() << "' in iteration " << iteration << std::endl;
         ++failed;
      }
      // LCOV_EXCL_STOP
   }

}  // namespace tao::config

int main()
{
   for( const auto& entry : std::filesystem::directory_iterator( "tests" ) ) {
      if( const auto& path = entry.path(); path.extension() == ".success" ) {
#if defined( _MSC_VER )
         if( entry.path().stem() == "shell" ) {
            continue;
         }
#endif
         tao::config::internal::setenv_throws( "TAO_CONFIG", "env_value" );
         tao::config::unit_test( path );
      }
   }
   if( tao::config::failed == 0 ) {
      std::cerr << "All " << tao::config::iteration << " order independence testcases passed." << std::endl;
   }
   return std::min( int( tao::config::failed ), 127 );
}
