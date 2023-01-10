// Copyright (c) 2019-2023 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_INTERNAL_PHASE2_GUARD_HPP
#define TAO_CONFIG_INTERNAL_PHASE2_GUARD_HPP

#include <cassert>
#include <set>

#include "pegtl.hpp"

namespace tao::config::internal
{
   class [[nodiscard]] phase2_guard
   {
   public:
      template< typename T >
      phase2_guard( std::set< const void* >& stack, T& thing )
         : m_stack( stack ),
           m_thing( &thing )
      {
         if( !m_stack.insert( m_thing ).second ) {
            throw pegtl::parse_error( "reference cycle detected", thing.position );
         }
      }

      phase2_guard( phase2_guard&& ) = delete;
      phase2_guard( const phase2_guard& ) = delete;

      ~phase2_guard()
      {
         const std::size_t count = m_stack.erase( m_thing );
         assert( count == 1 );
         (void)count;
      }

      void operator=( phase2_guard&& ) = delete;
      void operator=( const phase2_guard& ) = delete;

   private:
      std::set< const void* >& m_stack;
      const void* m_thing;
   };

}  // namespace tao::config::internal

#endif
