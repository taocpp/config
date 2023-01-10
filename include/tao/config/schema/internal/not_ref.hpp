// Copyright (c) 2019-2023 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_SCHEMA_INTERNAL_NOT_REF_HPP
#define TAO_CONFIG_SCHEMA_INTERNAL_NOT_REF_HPP

#include "ref.hpp"

namespace tao::config::schema::internal
{
   struct not_ref : ref
   {
      using ref::ref;

      json::value validate( const value& v ) const override
      {
         return ref::validate( v ) ? ok() : error( v, "unexpected match" );
      }
   };

}  // namespace tao::config::schema::internal

#endif
