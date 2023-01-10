// Copyright (c) 2018-2023 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_INTERNAL_EXTENSION_UTILITY_HPP
#define TAO_CONFIG_INTERNAL_EXTENSION_UTILITY_HPP

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
   template< typename R >
   [[nodiscard]] json_t convert_result( const pegtl::position& p, R&& r )
   {
      return result_traits< std::decay_t< R > >::convert( p, std::forward< R >( r ) );
   }

   template< typename R, typename A >
   [[nodiscard]] value_extension wrap( R ( *f )( A ) )
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
   [[nodiscard]] value_extension wrap( R ( *f )( A, B ) )
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

   template< typename R, typename A, typename B, typename C >
   [[nodiscard]] value_extension wrap( R ( *f )( A, B, C ) )
   {
      static_assert( !std::is_pointer_v< R > );
      static_assert( !std::is_reference_v< R > );
      static_assert( !std::is_same_v< R, void > );

      return [ = ]( pegtl_input_t& in, state& st, const extension_maps& em ) {
         const auto p = in.position();
         const argument_traits< std::decay_t< A > > a( in, st, em );
         const argument_traits< std::decay_t< B > > b( in, st, em );
         const argument_traits< std::decay_t< C > > c( in, st, em );
         return convert_result( p, f( a.convert(), b.convert(), c.convert() ) );
      };
   }

   template< typename R, typename A, typename B, typename C, typename D >
   [[nodiscard]] value_extension wrap( R ( *f )( A, B, C, D ) )
   {
      static_assert( !std::is_pointer_v< R > );
      static_assert( !std::is_reference_v< R > );
      static_assert( !std::is_same_v< R, void > );

      return [ = ]( pegtl_input_t& in, state& st, const extension_maps& em ) {
         const auto p = in.position();
         const argument_traits< std::decay_t< A > > a( in, st, em );
         const argument_traits< std::decay_t< B > > b( in, st, em );
         const argument_traits< std::decay_t< C > > c( in, st, em );
         const argument_traits< std::decay_t< D > > d( in, st, em );
         return convert_result( p, f( a.convert(), b.convert(), c.convert(), d.convert() ) );
      };
   }

   template< typename A, typename B >
   [[nodiscard]] member_extension wrap( void ( *f )( A, B ) )
   {
      return [ = ]( pegtl_input_t& in, state& st, const extension_maps& em ) {
         const argument_traits< std::decay_t< A > > a( in, st, em );
         const argument_traits< std::decay_t< B > > b( in, st, em );
         f( a.convert(), b.convert() );
      };
   }

   template< typename A, typename B, typename C >
   [[nodiscard]] member_extension wrap( void ( *f )( A, B, C ) )
   {
      return [ = ]( pegtl_input_t& in, state& st, const extension_maps& em ) {
         const argument_traits< std::decay_t< A > > a( in, st, em );
         const argument_traits< std::decay_t< B > > b( in, st, em );
         const argument_traits< std::decay_t< C > > c( in, st, em );
         f( a.convert(), b.convert(), c.convert() );
      };
   }

   template< typename A, typename B, typename C, typename D >
   [[nodiscard]] member_extension wrap( void ( *f )( A, B, C, D ) )
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
