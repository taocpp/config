// Copyright (c) 2019-2023 Dr. Colin Hirsch and Daniel Frey
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)

#ifndef TAO_CONFIG_INTERNAL_CHANGE_ACTION_AND_STATES_HPP
#define TAO_CONFIG_INTERNAL_CHANGE_ACTION_AND_STATES_HPP

#include <tuple>
#include <utility>

#include "pegtl.hpp"

namespace tao::config::internal
{
   template< template< typename... > class NewAction, typename... NewStates >
   struct change_action_and_states
      : pegtl::maybe_nothing
   {
      template< typename Rule,
                pegtl::apply_mode A,
                pegtl::rewind_mode M,
                template< typename... >
                class Action,
                template< typename... >
                class Control,
                std::size_t... Ns,
                typename ParseInput,
                typename... States >
      [[nodiscard]] static bool match( std::index_sequence< Ns... > /*unused*/, ParseInput& in, States&&... st )
      {
         auto t = std::tie( st... );
         const auto pos = in.position();
         if( Control< Rule >::template match< A, M, NewAction, Control >( in, std::get< Ns >( t )... ) ) {
            if constexpr( A == pegtl::apply_mode::action ) {
               Action< Rule >::success( pos, st... );
            }
            return true;
         }
         return false;
      }

      template< typename Rule,
                pegtl::apply_mode A,
                pegtl::rewind_mode M,
                template< typename... >
                class Action,
                template< typename... >
                class Control,
                typename ParseInput,
                typename... States >
      [[nodiscard]] static bool match( ParseInput& in, States&&... st )
      {
         static_assert( !std::is_same_v< Action< void >, NewAction< void > >, "old and new action class templates are identical" );
         return match< Rule, A, M, Action, Control >( std::index_sequence_for< NewStates... >(), in, NewStates()..., st... );
      }
   };

}  // namespace tao::config::internal

#endif
