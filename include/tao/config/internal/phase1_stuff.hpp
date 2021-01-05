// Copyright (c) 2018-2020 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_INTERNAL_PHASE1_STUFF_HPP
#define TAO_CONFIG_INTERNAL_PHASE1_STUFF_HPP

#include <cassert>
#include <type_traits>

namespace tao::config::internal
{
   enum class phase1_stuff : char
   {
      remove_all,
      ensure_array,
      ensure_object,
      make_permanent,
      make_temporary
   };

   [[nodiscard]] inline bool is_implicit( const phase1_stuff s ) noexcept
   {
      switch( s ) {
         case phase1_stuff::remove_all:
            return true;
         case phase1_stuff::ensure_array:
         case phase1_stuff::ensure_object:
            return false;
         case phase1_stuff::make_permanent:
         case phase1_stuff::make_temporary:
            return true;
      }
      assert( false );  // UNREACHABLE
   }

   template< typename T >
   [[nodiscard]] inline bool is_implicit_any( const T& t ) noexcept
   {
      if constexpr( std::is_same_v< T, phase1_stuff > ) {
         return is_implicit( t );
      }
      return false;
   }

}  // namespace tao::config::internal

#endif
