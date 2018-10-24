// Copyright (c) 2018 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_INTERNAL_CHANGES_HPP
#define TAO_CONFIG_INTERNAL_CHANGES_HPP

#include <type_traits>

#include "pegtl.hpp"

namespace tao
{
   namespace config
   {
      namespace internal
      {
         struct dummy_disabled_state
         {
            template< typename Input >
            dummy_disabled_state( const Input& /*unused*/ ) noexcept
            {
            }

            template< typename Input, typename... Ts >
            void success( const Input& /*unused*/, Ts&&... /*unused*/ ) const noexcept
            {
            }
         };

         template< pegtl::apply_mode A, typename State >
         using state_disable_helper = std::conditional_t< A == pegtl::apply_mode::ACTION, State, dummy_disabled_state >;

         template< typename Rule, typename State, template< typename... > class Base = pegtl::normal >
         struct change_state
            : public Base< Rule >
         {
            template< pegtl::apply_mode A,
                      pegtl::rewind_mode M,
                      template< typename... > class Action,
                      template< typename... > class Control,
                      typename Input,
                      typename... States >
            static bool match( Input& in, States&&... st )
            {
               internal::state_disable_helper< A, State > s( static_cast< const Input& >( in ) );

               if( Base< Rule >::template match< A, M, Action, Control >( in, s ) ) {
                  s.success( static_cast< const Input& >( in ), st... );
                  return true;
               }
               return false;
            }
         };

         template< typename Rule, template< typename... > class Action, template< typename... > class Base = pegtl::normal >
         struct change_action
            : public Base< Rule >
         {
            template< pegtl::apply_mode A,
                      pegtl::rewind_mode M,
                      template< typename... > class,
                      template< typename... > class Control,
                      typename Input,
                      typename... States >
            static bool match( Input& in, States&&... st )
            {
               return Base< Rule >::template match< A, M, Action, Control >( in, st... );
            }
         };

         template< template< typename... > class Action, template< typename... > class Base >
         struct change_both_helper
         {
            template< typename T >
            using change_action = change_action< T, Action, Base >;
         };

         template< typename Rule, typename State, template< typename... > class Action, template< typename... > class Base = pegtl::normal >
         struct change_state_and_action
            : public change_state< Rule, State, change_both_helper< Action, Base >::template change_action >
         {
         };

      }  // namespace internal

   }  // namespace config

}  // namespace tao

#endif
