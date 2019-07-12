// Copyright (c) 2018-2019 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_INTERNAL_FORMAT_HPP
#define TAO_CONFIG_INTERNAL_FORMAT_HPP

#include <sstream>

#include "format_traits.hpp"
#include "json.hpp"

namespace tao::config::internal
{
   inline std::string format( const char* function, const char* message, std::initializer_list< json::internal::pair< format_traits > >&& l )
   {
      std::ostringstream os;
      os << "'" << message << "' -- " << json::basic_value< format_traits >( std::move( l ) ) << " -- " << function;
      return os.str();
   }

}  // namespace tao::config::internal

#endif
