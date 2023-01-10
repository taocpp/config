// Copyright (c) 2019-2023 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_SCHEMA_INTERNAL_LIST_HPP
#define TAO_CONFIG_SCHEMA_INTERNAL_LIST_HPP

#include "node.hpp"

namespace tao::config::schema::internal
{
   template< typename T, typename E >
   struct list : T
   {
      template< typename... Ts >
      explicit list( const value& v, Ts&&... ts )
         : T( v )
      {
         for( const auto& e : v.get_array() ) {
            this->m_properties.emplace_back( std::make_shared< E >( e, ts... ) );
         }
      }
   };

}  // namespace tao::config::schema::internal

#endif
