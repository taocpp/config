// Copyright (c) 2019-2023 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_INTERNAL_JSON_BASE_HPP
#define TAO_CONFIG_INTERNAL_JSON_BASE_HPP

#include <ostream>
#include <utility>

#include "pegtl.hpp"

namespace tao::config::internal
{
   struct json_base
   {
      pegtl::position position;

      json_base()
         : position( pegtl::internal::inputerator{ nullptr, 1, 1, 1 }, "TODO: Delete the default c'tor!" )
      {}
      //      json_base() = delete;

      json_base( json_base&& ) = default;
      json_base( const json_base& ) = default;

      json_base& operator=( json_base&& ) = default;
      json_base& operator=( const json_base& ) = default;

      json_base( pegtl::position&& p ) noexcept
         : position( std::move( p ) )
      {}

      json_base( const pegtl::position& p )
         : position( p )
      {}

      void append_message_extension( std::ostream& o ) const
      {
         o << '(' << position.source << ':' << position.line << ':' << position.column << ')';
      }
   };

}  // namespace tao::config::internal

#endif
