// Copyright (c) 2019-2023 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_SCHEMA_INTERNAL_REF_IMPL_HPP
#define TAO_CONFIG_SCHEMA_INTERNAL_REF_IMPL_HPP

#include "any_of.hpp"
#include "list.hpp"
#include "ref.hpp"
#include "schema.hpp"
#include "trivial.hpp"

namespace tao::config::schema::internal
{
   inline ref::ref( const value& v, node_map& m, const std::string& path )
      : node( v )
   {
      if( v.is_boolean() ) {
         if( v.as< bool >() ) {
            m_node = std::make_shared< trivial< true > >( v );
         }
         else {
            m_node = std::make_shared< trivial< false > >( v );
         }
      }
      else if( v.is_array() ) {
         m_node = std::make_shared< list< any_of, ref > >( v, m, path );
      }
      else if( v.is_object() ) {
         m_node = std::make_shared< schema >( v, m, path );
      }
   }

}  // namespace tao::config::schema::internal

#endif
