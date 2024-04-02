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
   [[nodiscard]] function wrap( R ( *x )( const pegtl::position&, A ) )
   {
      static_assert( !std::is_pointer_v< R > );
      static_assert( !std::is_reference_v< R > );
      static_assert( !std::is_same_v< R, void > );

      return function( [ x ]( entry& e ) {
         try {
            array& f = e.get_array();
            function_traits< std::decay_t< R > >::put( e, x( f.position, function_traits< std::decay_t< A > >::get( f, 0 ) ) );
         }
         catch( const arguments_unready& ) {
            return false;
         }
         return true;
      } );
   }

   template< typename R, typename A, typename B >
   [[nodiscard]] function wrap( R ( *x )( const pegtl::position&, A, B ) )
   {
      static_assert( !std::is_pointer_v< R > );
      static_assert( !std::is_reference_v< R > );
      static_assert( !std::is_same_v< R, void > );

      return function( [ x ]( entry& e ) {
         try {
            array& f = e.get_array();
            function_traits< std::decay_t< R > >::put( e, x( f.position, function_traits< std::decay_t< A > >::get( f, 0 ), function_traits< std::decay_t< B > >::get( f, 1 ) ) );
         }
         catch( const arguments_unready& ) {
            return false;
         }
         return true;
      } );
   }

}  // namespace tao::config::internal

#endif
