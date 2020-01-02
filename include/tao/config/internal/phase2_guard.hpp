// Copyright (c) 2019-2020 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_INTERNAL_PHASE2_GUARD_HPP
#define TAO_CONFIG_INTERNAL_PHASE2_GUARD_HPP

namespace tao::config::internal
{
   template< typename T >
   class phase2_guard
   {
   public:
      explicit phase2_guard( T& v )
         : m_v( v )
      {
         m_v.set_recursion_marker();
      }

      ~phase2_guard()
      {
         m_v.clear_recursion_marker();
      }

      phase2_guard( phase2_guard&& ) = delete;
      phase2_guard( const phase2_guard& ) = delete;

      void operator=( phase2_guard&& ) = delete;
      void operator=( const phase2_guard& ) = delete;

   private:
      T& m_v;
   };

   template< typename T >
   phase2_guard( T& )->phase2_guard< T >;

}  // namespace tao::config::internal

#endif
