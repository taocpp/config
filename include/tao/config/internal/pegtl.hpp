// Copyright (c) 2018-2020 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_INTERNAL_PEGTL_HPP
#define TAO_CONFIG_INTERNAL_PEGTL_HPP

#include <tao/json/external/pegtl.hpp>

namespace tao::json::pegtl
{
   namespace internal
   {
      template< typename F, F U >
      struct function;

      template< typename ParseInput, typename... States, bool( *U )( ParseInput&, States... ) >
      struct function< bool( * )( ParseInput&, States... ), U >
      {
         template< pegtl::apply_mode A,
                   pegtl::rewind_mode M,
                   template< typename... > class Action,
                   template< typename... > class Control >
         [[nodiscard]] static bool match( ParseInput& in, States... st ) noexcept( noexcept( U( in, st... ) ) )
         {
            return U( in, st... );
         }
      };

      template< typename F, F U >
      inline constexpr bool enable_control< function< F, U > > = false;

   }  // namespace internal

   template< auto F > using function = internal::function< decltype( F ), F >;

   // TODO: Move "instantiate" to PEGTL contrib?

   template< typename T >
   struct instantiate
      : maybe_nothing
   {
      template< typename Rule,
                apply_mode A,
                rewind_mode M,
                template< typename... >
                class Action,
                template< typename... >
                class Control,
                typename ParseInput,
                typename... States >
      [[nodiscard]] static bool match( ParseInput& in, States&... st )
      {
         const T t( static_cast< const ParseInput& >( in ), st... );
         return pegtl::match< Rule, A, M, Action, Control >( in, st... );
      }
   };

}  // namespace tao::json::pegtl

namespace tao::config::internal
{
   namespace pegtl = json::pegtl;

   using pegtl_input_t = pegtl::memory_input< pegtl::tracking_mode::eager, pegtl::eol::lf_crlf >;

}  // namespace tao::config::internal

#endif
