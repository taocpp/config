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
         json::position position;  // TODO: json::position, pegtl::position or TBD config::position?

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

         void set_pointer( const config::pointer& ptr )  // TODO: Use internal::pointer here?
         {
            pointer = ptr;
         }

         void set_position( const json_pegtl::position& pos )
         {
            position.set_position( pos );
         }
      };

   }  // namespace config

}  // namespace tao

#endif
