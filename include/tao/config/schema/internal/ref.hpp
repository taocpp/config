// Copyright (c) 2019-2023 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_SCHEMA_INTERNAL_REF_HPP
#define TAO_CONFIG_SCHEMA_INTERNAL_REF_HPP

#include <sstream>
#include <string>
#include <utility>

#include "node.hpp"

namespace tao::config::schema::internal
{
   struct ref : node
   {
      std::shared_ptr< node > m_node;
      const node* m_ptr = nullptr;

      ref( const value& v, node_map& m, const std::string& path );

      void resolve( const node_map& m ) override
      {
         if( m_node ) {
            m_node->resolve( m );
            m_ptr = m_node.get();
         }
         else {
            const auto k = m_source.as< std::string >();
            const auto it = m.find( k );
            if( it == m.end() ) {
               std::ostringstream oss;
               oss << "can not resolve '" << k << "' defined here:";
               m_source.append_message_extension( oss );
               throw std::runtime_error( std::move( oss ).str() );
            }
            m_ptr = it->second.get();
         }
      }

      json::value validate( const value& v ) const override
      {
         return m_ptr->validate( v );
      }

      json::value pos() const override
      {
         auto result = m_ptr->pos();
         auto via = node::pos();
         if( result != via ) {
            append_via( result, std::move( via ) );
         }
         return result;
      }
   };

}  // namespace tao::config::schema::internal

#endif
