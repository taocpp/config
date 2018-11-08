// Copyright (c) 2018 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_INTERNAL_POINTER_HPP
#define TAO_CONFIG_INTERNAL_POINTER_HPP

#include <sstream>
#include <vector>

#include "token.hpp"
#include "traits.hpp"

namespace tao
{
   namespace config
   {
      namespace internal
      {
         using pointer = std::vector< token >;

         inline pointer& operator+=( pointer& l, const token::kind k )
         {
            l.emplace_back( k );
            return l;
         }

         inline pointer& operator+=( pointer& l, const std::size_t i )
         {
            l.emplace_back( i );
            return l;
         }

         inline pointer& operator+=( pointer& l, const std::string& n )
         {
            l.emplace_back( n );
            return l;
         }

         inline pointer operator+( const pointer& l, const token::kind k )
         {
            pointer r = l;
            r += k;
            return r;
         }

         inline pointer operator+( const pointer& l, const std::size_t i )
         {
            pointer r = l;
            r += i;
            return r;
         }

         inline pointer operator+( const pointer& l, const std::string& n )
         {
            pointer r = l;
            r += n;
            return r;
         }

         inline pointer pop_front( const pointer& p )
         {
            assert( !p.empty() );

            return pointer( p.begin() + 1, p.end() );
         }

         inline pointer pop_back( const pointer& p )
         {
            assert( !p.empty() );

            return pointer( p.begin(), p.end() - 1 );
         }

         inline void pointer_to_stream( std::ostream& o, const pointer& p )
         {
            if( !p.empty() ) {
               token_to_stream( o, p[ 0 ] );

               for( std::size_t i = 1; i < p.size(); ++i ) {
                  o << '.';
                  token_to_stream( o, p[ i ] );
               }
            }
         }

         inline std::string pointer_to_string( const pointer& p )
         {
            std::ostringstream o;
            pointer_to_stream( o, p );
            return o.str();
         }

         template<>
         struct traits< pointer >
         {
            template< template< typename... > class Traits, typename Consumer >
            static void produce( Consumer& c, const pointer& p )
            {
               c.string( pointer_to_string( p ) );
            }
         };

         template<>
         struct traits< const pointer* >
         {
            template< template< typename... > class Traits >
            static void assign( json::basic_value< Traits >& v, const pointer* p )
            {
               v.unsafe_assign_opaque_ptr( p );
            }
         };

         template<>
         struct traits< pointer* >
            : public traits< const pointer* >
         {
         };

      }  // namespace internal

   }  // namespace config

}  // namespace tao

#endif
