// Copyright (c) 2019-2023 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_SCHEMA_INTERNAL_FORWARD_HPP
#define TAO_CONFIG_SCHEMA_INTERNAL_FORWARD_HPP

#include <filesystem>

#include <tao/json.hpp>

#include "../../value.hpp"

#include "../builtin.hpp"

namespace tao::config::schema::internal
{
   inline json::value phase2_from_file_and_validate( const std::filesystem::path& schema, const builtin& b, const value& config_value );

}  // namespace tao::config::schema::internal

#endif
