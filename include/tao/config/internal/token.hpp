// Copyright (c) 2018 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_INTERNAL_TOKEN_HPP
#define TAO_CONFIG_INTERNAL_TOKEN_HPP

#include <cassert>
#include <string>

namespace tao
{
   namespace config
   {
      namespace internal
      {
         class token
         {
         public:
            enum kind : char
            {
               NAME,
               INDEX,
               STAR,
               MINUS
            };

            explicit
            token( const kind t )
               : m_type( t )
            {
               assert( ( t == STAR ) || ( t == MINUS ) );
            }

            explicit
            token( const std::size_t index )
               : m_type( kind::INDEX ),
                 m_index( index )
            {
            }

            explicit
            token( const std::string& name )
               : m_type( NAME ),
                 m_name( name )
            {
            }

            kind type() const noexcept
            {
               return m_type;
            }

            std::size_t index() const noexcept
            {
               assert( m_type == INDEX );

               return m_index;
            }

            const std::string& name() const noexcept
            {
               assert( m_type == NAME );

               return m_name;
            }

         private:
            kind m_type;

            // Only one of the following is valid, but that is accepatble for
            // the moment because this is a config reader, not HPC, and it is
            // only in the internal representation, not exposed to the user.
            std::size_t m_index;
            std::string m_name;
         };

      }  // namespace internal

   }  // namespace config

}  // namespace tao

#endif
