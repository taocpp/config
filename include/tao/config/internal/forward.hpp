// Copyright (c) 2019 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_INTERNAL_FORWARD_HPP
#define TAO_CONFIG_INTERNAL_FORWARD_HPP

namespace tao::config::schema
{
   struct builtin;
   struct validator;

   inline json::value parse_and_validate( const std::string& schema_filename, const value& config_value );

}  // namespace tao::config::schema

namespace tao::config::internal
{
   class entry;
   class entry_array;
   class entry_object;

   template< typename T >
   struct value_traits;

   using json_t = tao::json::basic_value< value_traits >;

}  // namespace tao::config::internal

#endif
