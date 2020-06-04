// Copyright (c) 2019-2020 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_INTERNAL_FORWARD_HPP
#define TAO_CONFIG_INTERNAL_FORWARD_HPP

#include <filesystem>

namespace tao::config::schema
{
   struct builtin;
   struct validator;

   namespace internal
   {
      // TODO: Where are json::value and value coming from??
      inline json::value phase2_from_file_and_validate( const std::filesystem::path& schema, builtin b, const value& config_value );

   }  // namespace internal

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
