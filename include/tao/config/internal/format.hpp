// Copyright (c) 2018-2019Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_INTERNAL_FORMAT_HPP
#define TAO_CONFIG_INTERNAL_FORMAT_HPP

#include <sstream>

#include "json.hpp"
#include "traits.hpp"

namespace tao
{
   namespace config
   {
      namespace internal
      {
         inline std::string format( const std::string& message, std::initializer_list< json::internal::pair< traits > >&& l )
         {
            std::ostringstream os;
            os << "'" << message << "' -- " << json::basic_value< traits >( std::move( l ) );
            return os.str();
         }

      }  // namespace internal

   }  // namespace config

}  // namespace tao

#endif
