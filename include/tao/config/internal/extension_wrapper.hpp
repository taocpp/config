// Copyright (c) 2018-2024 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_INTERNAL_EXTENSION_WRAPPER_HPP
#define TAO_CONFIG_INTERNAL_EXTENSION_WRAPPER_HPP

#include <type_traits>
#include <utility>

#include "argument_traits.hpp"
#include "extension_maps.hpp"
#include "extension_types.hpp"
#include "forward.hpp"
#include "json.hpp"
#include "json_traits.hpp"
#include "pegtl.hpp"
#include "result_traits.hpp"

namespace tao::config::internal
{
   // NOTE: We can't just make wrap() variadic the straightforward way because that would look like
   // return convert_result( p, f( argument_traits< std::decay_t< As > >( in, st, em ).convert()... ) );
   // and with the order of function parameter evaluation not being defined in C++ that doesn't work.

   template< typename R, typename A >
   [[nodiscard]] inner_extension wrap( R ( *f )( A ) )
   {
      static_assert( !std::is_pointer_v< R > );
      static_assert( !std::is_reference_v< R > );
      static_assert( !std::is_same_v< R, void > );

      return [ = ]( pegtl_input_t& in, state& st, const extension_maps& em ) {
         const auto p = in.position();
         const argument_traits< std::decay_t< A > > a( in, st, em );
         return convert_result( p, f( a.convert() ) );
      };
   }

   template< typename R, typename A, typename B >
   [[nodiscard]] inner_extension wrap( R ( *f )( A, B ) )
   {
      static_assert( !std::is_pointer_v< R > );
      static_assert( !std::is_reference_v< R > );
      static_assert( !std::is_same_v< R, void > );

      return [ = ]( pegtl_input_t& in, state& st, const extension_maps& em ) {
         const auto p = in.position();
         const argument_traits< std::decay_t< A > > a( in, st, em );
         const argument_traits< std::decay_t< B > > b( in, st, em );
         return convert_result( p, f( a.convert(), b.convert() ) );
      };
   }

   template< typename A, typename B >
   [[nodiscard]] outer_extension wrap( void ( *f )( A, B ) )
   {
      return [ = ]( pegtl_input_t& in, state& st, const extension_maps& em ) {
         const argument_traits< std::decay_t< A > > a( in, st, em );
         const argument_traits< std::decay_t< B > > b( in, st, em );
         f( a.convert(), b.convert() );
      };
   }

   template< typename A, typename B, typename C >
   [[nodiscard]] outer_extension wrap( void ( *f )( A, B, C ) )
   {
      return [ = ]( pegtl_input_t& in, state& st, const extension_maps& em ) {
         const argument_traits< std::decay_t< A > > a( in, st, em );
         const argument_traits< std::decay_t< B > > b( in, st, em );
         const argument_traits< std::decay_t< C > > c( in, st, em );
         f( a.convert(), b.convert(), c.convert() );
      };
   }

   template< typename A, typename B, typename C, typename D >
   [[nodiscard]] outer_extension wrap( void ( *f )( A, B, C, D ) )
   {
      return [ = ]( pegtl_input_t& in, state& st, const extension_maps& em ) {
         const argument_traits< std::decay_t< A > > a( in, st, em );
         const argument_traits< std::decay_t< B > > b( in, st, em );
         const argument_traits< std::decay_t< C > > c( in, st, em );
         const argument_traits< std::decay_t< D > > d( in, st, em );
         f( a.convert(), b.convert(), c.convert(), d.convert() );
      };
   }

}  // namespace tao::config::internal

#endif
