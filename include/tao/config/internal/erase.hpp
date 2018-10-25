// Copyright (c) 2018 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_INTERNAL_ERASE_HPP
#define TAO_CONFIG_INTERNAL_ERASE_HPP

#include <stdexcept>

#include "pointer.hpp"
#include "utility.hpp"
#include "entry.hpp"

namespace tao
{
   namespace config
   {
      namespace internal
      {
         inline std::size_t erase( concat& l, const pointer& p, const token& f );

         inline std::size_t erase_name( concat& l, const std::string& k )
         {
            std::size_t r = 0;

            for( auto& i : l.v ) {
               if( !i.is_object() ) {
                  throw std::runtime_error( std::string( __FILE__ ) + ":" + std::to_string( __LINE__ ) );
               }
               r += i.get_object().erase( k );
            }
            return r;
         }

         inline std::size_t erase_index( concat& l, std::size_t n )
         {
            for( auto& i : l.v ) {
               if( !i.is_array() ) {
                  throw std::runtime_error( std::string( __FILE__ ) + ":" + std::to_string( __LINE__ ) );
               }
               auto& a = i.get_array();
               const auto s = a.size();

               if( n < s ) {
                  a.erase( a.begin() + n );
                  return 1;
               }
               n -= s;
            }
            throw std::runtime_error( std::string( __FILE__ ) + ":" + std::to_string( __LINE__ ) );
         }

         inline std::size_t erase_star( concat& l )
         {
            for( auto& i : l.v ) {
               if( i.is_array() ) {
                  const std::size_t r = i.get_array().size();
                  i.get_array().clear();
                  return r;
               }
               else if( i.is_object() ) {
                  const std::size_t r = i.get_object().size();
                  i.get_object().clear();
                  return r;
               }
               else {
                  throw std::runtime_error( std::string( __FILE__ ) + ":" + std::to_string( __LINE__ ) );
               }
            }
            return 0;
         }

         inline std::size_t erase_minus( concat& l )
         {
            for( auto& i : reverse( l.v ) ) {
               if( !i.is_array() ) {
                  throw std::runtime_error( std::string( __FILE__ ) + ":" + std::to_string( __LINE__ ) );
               }
               auto& a = i.get_array();

               if( !a.empty() ) {
                  a.pop_back();
                  return 1;
               }
            }
            throw std::runtime_error( std::string( __FILE__ ) + ":" + std::to_string( __LINE__ ) );
         }

         inline std::size_t erase( concat& l, const token& f )
         {
            switch( f.type() ) {
               case token::NAME:
                  return erase_name( l, f.name() );
               case token::INDEX:
                  return erase_index( l, f.index() );
               case token::STAR:
                  return erase_star( l );
               case token::MINUS:
                  return erase_minus( l );
            }
            assert( false );
         }

         inline std::size_t erase_name( concat& l, const std::string& k, const pointer& p, const token& f )
         {
            std::size_t r = 0;

            for( auto& i : reverse( l.v ) ) {
               if( !i.is_object() ) {
                  throw std::runtime_error( std::string( __FILE__ ) + ":" + std::to_string( __LINE__ ) );
               }
               const auto j = i.get_object().find( k );

               if( j != i.get_object().end() ) {
                  r += erase( j->second, p, f );
               }
            }
            return r;
         }

         inline std::size_t erase_index( concat& l, std::size_t n, const pointer& p, const token& f )
         {
            for( auto& i : l.v ) {
               if( !i.is_array() ) {
                  throw std::runtime_error( std::string( __FILE__ ) + ":" + std::to_string( __LINE__ ) );
               }
               const auto s = i.get_array().size();

               if( n < s ) {
                  return erase( i.get_array()[ n ], p, f );
               }
               n -= s;
            }
            throw std::runtime_error( std::string( __FILE__ ) + ":" + std::to_string( __LINE__ ) );
         }

         inline std::size_t erase_minus( concat& l, const pointer& p, const token& f )
         {
            for( auto& i : reverse( l.v ) ) {
               if( !i.is_array() ) {
                  throw std::runtime_error( std::string( __FILE__ ) + ":" + std::to_string( __LINE__ ) );
               }
               if( !i.get_array().empty() ) {
                  return erase( i.get_array().back(), p, f );
               }
            }
            throw std::runtime_error( std::string( __FILE__ ) + ":" + std::to_string( __LINE__ ) );
         }

         inline std::size_t erase( concat& l, const token& t, const pointer& p, const token& f )
         {
            switch( t.type() ) {
               case token::NAME:
                  return erase_name( l, t.name(), p, f );
               case token::INDEX:
                  return erase_index( l, t.index(), p, f );
               case token::STAR:
                  assert( false );
               case token::MINUS:
                  return erase_minus( l, p, f );
            }
            assert( false );
         }

         inline std::size_t erase( concat& l, const pointer& p, const token& f )
         {
            if( p.empty() ) {
               return erase( l, f );
            }
            else {
               return erase( l, p.front(), pop_front( p ), f );
            }
         }

         inline std::size_t erase( object_t& o, const std::string& k, const pointer& p )
         {
            if( p.empty() ) {
               return o.erase( k );
            }
            const auto i = o.find( k );

            if( i != o.end() ) {
               return erase( i->second, pop_back( p ), p.back() );
            }
            return 0;
         }

         inline std::size_t erase( object_t& o, const token& t, const pointer& p )
         {
            switch( t.type() ) {
               case token::NAME:
                  return erase( o, t.name(), p );
               case token::INDEX:
                  assert( false );
               case token::STAR:
                  assert( false );
               case token::MINUS:
                  assert( false );
            }
            assert( false );
         }

         inline std::size_t erase( object_t& o, const pointer& p )
         {
            assert( !p.empty() );

            return erase( o, p.front(), pop_front( p ) );
         }

      }  // namespace internal

   }  // namespace config

}  // namespace tao

#endif
