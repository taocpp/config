// Copyright (c) 2018 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_INTERNAL_PHASE2_HPP
#define TAO_CONFIG_INTERNAL_PHASE2_HPP

#include "access.hpp"
#include "addition.hpp"
#include "json.hpp"
#include "utility.hpp"

namespace tao
{
   namespace config
   {
      namespace internal
      {
         template< template< typename... > class Traits >
         inline void phase2_pointers( json::basic_value< Traits >& r, const config::pointer& p )
         {
            switch( r.type() ) {
               case json::type::ARRAY:
                  for( std::size_t i = 0; i < r.unsafe_get_array().size(); ++i ) {
                     phase2_pointers( r[ i ], p + i );
                  }
                  break;
               case json::type::OBJECT:
                  for( auto& i : r.unsafe_get_object() ) {
                     phase2_pointers( i.second, p + i.first );
                  }
                  break;
               default:
                  break;
            }
            r.pointer = p;
         }

         class phase2_impl
         {
         public:
            explicit
            phase2_impl( const object_t& root )
               : m_root( root )
            {
            }

            template< template< typename... > class Traits >
            json::basic_value< Traits > phase2() const
            {
               json::basic_value< Traits > r = process_object< Traits >( m_root );
               phase2_pointers( r, config::pointer() );
               return r;
            }

         private:
            template< template< typename... > class Traits >
            json::basic_value< Traits > process_entry( const entry& e ) const
            {
               const phase2_guard p2g( e );

               switch( e.type() ) {
                  case entry::ATOM:
                     return process_atom< Traits >( e.get_atom() );
                  case entry::ARRAY:
                     return process_array< Traits >( e.get_array() );
                  case entry::OBJECT:
                     return process_object< Traits >( e.get_object() );
                  case entry::NOTHING:
                     throw std::runtime_error( "TODO - how could this happen?" );
                  case entry::INDIRECT:
                     return process_indirect< Traits >( e.get_indirect() );
               }
               assert( false );
            }

            template< template< typename... > class Traits >
            json::basic_value< Traits > process_list( const concat& l ) const
            {
               std::vector< json::basic_value< Traits > > t;

               for( const auto& i : l.v ) {
                  t.emplace_back( process_entry< Traits >( i ) );
               }
               return value_addition( t );
            }

            template< template< typename... > class Traits >
            json::basic_value< Traits > process_atom( const atom_t& v ) const
            {
               json::events::to_basic_value< Traits > consumer;
               json::events::from_value( consumer, v );
               return std::move( consumer.value );
            }

            template< template< typename... > class Traits >
            json::basic_value< Traits > process_array( const array_t& a ) const
            {
               json::basic_value< Traits > t = json::empty_array;

               for( const auto& i : a ) {
                  t.emplace_back( process_list< Traits >( i ) );
               }
               return t;
            }

            template< template< typename... > class Traits >
            json::basic_value< Traits > process_object( const object_t& o ) const
            {
               json::basic_value< Traits > t = json::empty_object;

               for( const auto& i : o ) {
                  t.emplace( i.first, process_list< Traits >( i.second ) );
               }
               return t;
            }

            const concat& process_reference( const indirect_t& r ) const
            {
               pointer p;

               for( auto& i : r.get_array() ) {
                  if( i.is_array() ) {
                     p.emplace_back( token_from_value( process_list< json::traits >( process_reference( i ) ) ) );
                  }
                  else {
                     p.emplace_back( token_from_value( i ) );
                  }
               }
               return access( m_root, p );
            }

            template< template< typename... > class Traits >
            json::basic_value< Traits > process_indirect( const indirect_t& r ) const
            {
               return process_list< Traits >( process_reference( r ) );
            }

         private:
            const object_t& m_root;
         };

         template< template< typename... > class Traits >
         json::basic_value< Traits > phase2( const object_t& v )
         {
            return phase2_impl( v ).phase2< Traits >();
         }

      }  // namespace internal

   }  // namespace config

}  // namespace tao

#endif
