// Copyright (c) 2018 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_ANNOTATION_HPP
#define TAO_CONFIG_ANNOTATION_HPP

#include "external/json.hpp"

namespace tao
{
   namespace config
   {
      struct annotation
      {
         config::pointer pointer;
         json::position position;  // TODO: json::position, pegtl::position or config::position?

         // TODO: Can we use binding::object without these functions?

         std::size_t line() const noexcept
         {
            return position.line();
         }

         std::size_t byte_in_line() const noexcept
         {
            return position.byte_in_line();
         }

         const std::string& source() const noexcept
         {
            return position.source();
         }
      };

   }  // namespace config

}  // namespace tao

#endif
