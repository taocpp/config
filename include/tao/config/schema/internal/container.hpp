// Copyright (c) 2019-2023 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_SCHEMA_INTERNAL_CONTAINER_HPP
#define TAO_CONFIG_SCHEMA_INTERNAL_CONTAINER_HPP

#include "node.hpp"

namespace tao::config::schema::internal
{
   struct container : node
   {
      using node::node;

      std::vector< std::shared_ptr< node > > m_properties;

      void resolve( const node_map& m ) override
      {
         for( const auto& p : m_properties ) {
            p->resolve( m );
         }
      }

      json::value pos() const override
      {
         if( m_properties.size() == 1 ) {
            auto result = m_properties.front()->pos();
            append_via( result, node::pos() );
            return result;
         }
         else {
            return node::pos();
         }
      }
   };

}  // namespace tao::config::schema::internal

#endif
