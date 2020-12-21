// Copyright (c) 2018-2020 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_INTERNAL_PEGTL_HPP
#define TAO_CONFIG_INTERNAL_PEGTL_HPP

#include <tao/json/external/pegtl.hpp>

#include <tao/json/external/pegtl/contrib/function.hpp>
#include <tao/json/external/pegtl/contrib/instantiate.hpp>
#include <tao/json/external/pegtl/contrib/predicates.hpp>

namespace tao::json::pegtl
{
   template< typename P >
   using invert = internal::predicates< internal::predicate_not_test, typename P::peek_t, P >;

}  // namespace tao::json::pegtl

namespace tao::config::internal
{
   namespace pegtl = json::pegtl;

   using pegtl_input_t = pegtl::memory_input< pegtl::tracking_mode::eager, pegtl::eol::lf_crlf >;

}  // namespace tao::config::internal

#endif
