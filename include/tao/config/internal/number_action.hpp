// Copyright (c) 2018-2020 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_INTERNAL_NUMBER_ACTION_HPP
#define TAO_CONFIG_INTERNAL_NUMBER_ACTION_HPP

#include "json.hpp"
#include "pegtl.hpp"

namespace tao::config::internal
{
   template< typename Rule >
   struct number_action
      : pegtl::nothing< Rule >
   {};

   template<>
   struct number_action< json::jaxn::internal::rules::hexnum< false > >
      : json::jaxn::internal::action< json::jaxn::internal::rules::hexnum< false > >
   {};

   template<>
   struct number_action< json::jaxn::internal::rules::hexnum< true > >
      : json::jaxn::internal::action< json::jaxn::internal::rules::hexnum< true > >
   {};

   template<>
   struct number_action< json::jaxn::internal::rules::zero< false > >
      : json::jaxn::internal::action< json::jaxn::internal::rules::zero< false > >
   {};

   template<>
   struct number_action< json::jaxn::internal::rules::zero< true > >
      : json::jaxn::internal::action< json::jaxn::internal::rules::zero< true > >
   {};

   template<>
   struct number_action< json::jaxn::internal::rules::kw_nan >
      : json::jaxn::internal::action< json::jaxn::internal::rules::kw_nan >
   {};

   template<>
   struct number_action< json::jaxn::internal::rules::kw_infinity< false > >
      : json::jaxn::internal::action< json::jaxn::internal::rules::kw_infinity< false > >
   {};

   template<>
   struct number_action< json::jaxn::internal::rules::kw_infinity< true > >
      : json::jaxn::internal::action< json::jaxn::internal::rules::kw_infinity< true > >
   {};

   template<>
   struct number_action< json::jaxn::internal::rules::esign >
      : json::jaxn::internal::action< json::jaxn::internal::rules::esign >
   {};

   template<>
   struct number_action< json::jaxn::internal::rules::idigits >
      : json::jaxn::internal::action< json::jaxn::internal::rules::idigits >
   {};

   template<>
   struct number_action< json::jaxn::internal::rules::fdigits >
      : json::jaxn::internal::action< json::jaxn::internal::rules::fdigits >
   {};

   template<>
   struct number_action< json::jaxn::internal::rules::edigits >
      : json::jaxn::internal::action< json::jaxn::internal::rules::edigits >
   {};

   template< bool Neg >
   struct number_action< json::jaxn::internal::rules::number< Neg > >
      : pegtl::change_states< json::internal::number_state< Neg > >
   {
      template< typename Input, typename Consumer >
      static void success( const Input& /*unused*/, json::internal::number_state< Neg >& state, Consumer& consumer )
      {
         state.success( consumer );
      }
   };

}  // namespace tao::config::internal

#endif
