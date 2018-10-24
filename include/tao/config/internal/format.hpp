// Copyright (c) 2018 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_INTERNAL_FORMAT_HPP
#define TAO_CONFIG_INTERNAL_FORMAT_HPP

#include <sstream>

#include "traits.hpp"

namespace tao
{
   namespace config
   {
      namespace internal
      {
         inline std::string format( const std::string& message, const json::basic_value< traits >& value )
         {
            std::ostringstream os;
            os << message << " " << value;
            return os.str();
         }

      }  // namespace internal

   }  // namespace config

}  // namespace tao

#endif
