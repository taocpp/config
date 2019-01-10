// Copyright (c) 2018 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_INTERNAL_CONCAT_HPP
#define TAO_CONFIG_INTERNAL_CONCAT_HPP

#include <vector>

#include "json.hpp"

namespace tao
{
   namespace config
   {
      namespace internal
      {
         class entry;

         class concat
         {
         public:
            explicit concat( const position& p )
               : p( p )
            {
            }

            bool is_temporary() const noexcept
            {
               return m_temporary;
            }

            void set_temporary( const bool t = true ) noexcept
            {
               m_temporary = t;
            }

            position p;
            std::vector< entry > v;

         private:
            mutable bool m_temporary = false;
         };

      }  // namespace internal

   }  // namespace config

}  // namespace tao

#endif
