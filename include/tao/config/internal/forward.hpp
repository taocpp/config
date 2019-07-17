#ifndef TAO_CONFIG_INTERNAL_FORWARD_HPP
#define TAO_CONFIG_INTERNAL_FORWARD_HPP

namespace tao::config::internal
{
   template< typename T > struct value_traits;

   using json_t = tao::json::basic_value< value_traits >;

}  // namespace tao::config::internal

#endif
