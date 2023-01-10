// Copyright (c) 2019-2023 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_SCHEMA_INTERNAL_SIZE_HPP
#define TAO_CONFIG_SCHEMA_INTERNAL_SIZE_HPP

#include "node.hpp"

namespace tao::config::schema::internal
{
   template< typename T >
   struct compare_size : node
   {
      const std::size_t m_size;

      explicit compare_size( const value& v )
         : node( v ),
           m_size( v.as< std::size_t >() )
      {}

      json::value validate( const value& v ) const override
      {
         if( v.is_string_type() ) {
            return T()( m_size, v.as< std::string_view >().size() ) ? ok() : error( v, "size did not match" );
         }
         if( v.is_binary_type() ) {
            return T()( m_size, v.as< tao::binary_view >().size() ) ? ok() : error( v, "size did not match" );
         }
         if( v.is_array() ) {
            return T()( m_size, v.get_array().size() ) ? ok() : error( v, "size did not match" );
         }
         if( v.is_object() ) {
            return T()( m_size, v.get_object().size() ) ? ok() : error( v, "size did not match" );
         }
         return error( v, "invalid type for size comparison" );
      }
   };

   using size = compare_size< std::equal_to<> >;
   using min_size = compare_size< std::less_equal<> >;
   using max_size = compare_size< std::greater_equal<> >;

}  // namespace tao::config::schema::internal

#endif
