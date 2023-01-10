// Copyright (c) 2018-2023 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_INTERNAL_REVERSE_HPP
#define TAO_CONFIG_INTERNAL_REVERSE_HPP

namespace tao::config::internal
{
   template< typename Container >
   struct reverse
   {
      explicit reverse( Container& l )
         : m_c( l )
      {}

      reverse( reverse&& ) = delete;
      reverse( const reverse& ) = delete;

      void operator=( reverse&& ) = delete;
      void operator=( const reverse& ) = delete;

      auto begin()
      {
         return m_c.rbegin();
      }

      auto end()
      {
         return m_c.rend();
      }

   private:
      Container& m_c;
   };

   template< typename T >
   reverse( T& ) -> reverse< T >;

}  // namespace tao::config::internal

#endif
