// Copyright (c) 2018-2024 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_INTERNAL_FUNCTION_WRAPPER_HPP
#define TAO_CONFIG_INTERNAL_FUNCTION_WRAPPER_HPP

#include <cstddef>
#include <type_traits>
#include <utility>

#include "forward.hpp"
#include "function_traits.hpp"
#include "json.hpp"
#include "json_traits.hpp"
#include "pegtl.hpp"

namespace tao::config::internal
{
   [[nodiscard]] inline function wrap( bool ( *x )( entry& e ) )
   {
      return function( [ x ]( entry& e ) {
         try {
            return x( e );
         }
         catch( const arguments_unready& ) {
            return false;
         }
      } );
   }

   template< typename R, typename A >
   [[nodiscard]] function wrap( R ( *x )( A ) )
   {
      static_assert( !std::is_pointer_v< R > );
      static_assert( !std::is_reference_v< R > );
      static_assert( !std::is_same_v< R, void > );

      return function( [ x ]( entry& e ) {
         array& f = e.get_array();
         try {
            std::size_t i = 0;
            decltype( auto ) a = function_traits< std::decay_t< A > >::get( f, i );
            function_traits< std::decay_t< R > >::put( e, x( a ) );
         }
         catch( const arguments_unready& ) {
            return false;
         }
         return true;
      } );
   }

   template< typename R, typename A, typename B >
   [[nodiscard]] function wrap( R ( *x )( A, B ) )
   {
      static_assert( !std::is_pointer_v< R > );
      static_assert( !std::is_reference_v< R > );
      static_assert( !std::is_same_v< R, void > );

      return function( [ x ]( entry& e ) {
         array& f = e.get_array();
         try {
            std::size_t i = 0;
            decltype( auto ) a = function_traits< std::decay_t< A > >::get( f, i );
            decltype( auto ) b = function_traits< std::decay_t< B > >::get( f, i );
            function_traits< std::decay_t< R > >::put( e, x( a, b ) );
         }
         catch( const arguments_unready& ) {
            return false;
         }
         return true;
      } );
   }

}  // namespace tao::config::internal

#endif
