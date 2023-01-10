// Copyright (c) 2019-2023 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_SCHEMA_INTERNAL_NODE_HPP
#define TAO_CONFIG_SCHEMA_INTERNAL_NODE_HPP

#include <map>
#include <memory>
#include <string>

#include "../../value.hpp"

#include "node_utility.hpp"

namespace tao::config::schema::internal
{
   struct node;

   using node_map = std::map< std::string, std::shared_ptr< node > >;

   struct node
   {
      value m_source;

      explicit node( const value& source )
         : m_source( source )
      {}

      virtual ~node() = default;

      virtual void resolve( const node_map& /*unused*/ ) {}
      virtual json::value validate( const value& v ) const = 0;

      virtual json::value pos() const
      {
         return internal::pos( m_source );
      }

      json::value error( const value& v, const char* message, json::value data = json::empty_object ) const
      {
         data.try_emplace( "_message", message );
         data.try_emplace( "_value", internal::pos( v ) );
         return data;
      }
   };

}  // namespace tao::config::schema::internal

#endif
