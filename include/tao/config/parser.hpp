// Copyright (c) 2022-2023 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_PARSER_HPP
#define TAO_CONFIG_PARSER_HPP

#include "internal/config_parser.hpp"

namespace tao::config
{
   class parser
   {
   public:
      parser() = default;

      void parse( const std::filesystem::path& path )
      {
         m_parser.parse( path );
      }

      void parse( const std::string_view data, const std::string& source )
      {
         m_parser.parse( data, source );
      }

      template< typename F >
      void set_inner_extension( const std::string& name, F& f )
      {
         m_parser.em.inner[ name ] = internal::wrap( f );
      }

      template< template< typename... > class Traits >
      [[nodiscard]] json::basic_value< Traits > result( const schema::builtin& b = schema::builtin() )
      {
         return m_parser.finish< Traits >( b );
      }

   protected:
      internal::config_parser m_parser;
   };

}  // namespace tao::config

#endif
