// Copyright (c) 2020 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_SRC_TEST_CONFIG_TEST_HPP
#define TAO_CONFIG_SRC_TEST_CONFIG_TEST_HPP

#include <cstddef>
#include <iostream>

namespace tao::config
{
   std::size_t failed = 0;

}  // namespace tao::config

#define TAO_TEST_STRINGIZE_INTERNAL( ... ) #__VA_ARGS__
#define TAO_TEST_STRINGIZE( ... ) TAO_TEST_STRINGIZE_INTERNAL( __VA_ARGS__ )

#define TAO_TEST_LINE TAO_TEST_STRINGIZE( __LINE__ )

#define TAO_CONFIG_TEST_UNWRAP( ... ) __VA_ARGS__

#define TAO_CONFIG_TEST_FAILED( MeSSaGe )            \
   do {                                              \
      std::cerr << "config: unit test failed for [ " \
                << tao::demangle< Rule >()           \
                << " ] "                             \
                << TAO_CONFIG_TEST_UNWRAP( MeSSaGe ) \
                << " in line [ "                     \
                << line                              \
                << " ] file [ "                      \
                << file << " ]"                      \
                << std::endl;                        \
      ++failed;                                      \
   } while( false )

#define TAO_CONFIG_TEST_ASSERT( ... )               \
   do {                                             \
      if( !( __VA_ARGS__ ) ) {                      \
         std::cerr << "config: unit test assert [ " \
                   << ( #__VA_ARGS__ )              \
                   << " ] failed in line [ "        \
                   << __LINE__                      \
                   << " ] file [ "                  \
                   << __FILE__ << " ]"              \
                   << std::endl;                    \
         ++failed;                                  \
      }                                             \
   } while( false )

#define TAO_CONFIG_TEST_THROWS( ... )               \
   do {                                             \
      try {                                         \
         __VA_ARGS__;                               \
         std::cerr << "config: unit test [ "        \
                   << ( #__VA_ARGS__ )              \
                   << " ] did not throw in line [ " \
                   << __LINE__                      \
                   << " ] file [ "                  \
                   << __FILE__ << " ]"              \
                   << std::endl;                    \
         ++failed;                                  \
      }                                             \
      catch( ... ) {                                \
      }                                             \
   } while( false )

#define TAO_CONFIG_TEST_UNREACHABLE                                                                                             \
   do {                                                                                                                         \
      std::cerr << "Code should be unreachable in " << __FUNCTION__ << " (" << __FILE__ << ':' << __LINE__ << ')' << std::endl; \
      std::abort();                                                                                                             \
   } while( false )

#endif
