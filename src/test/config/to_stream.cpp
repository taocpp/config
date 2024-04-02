// Copyright (c) 2024 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#include <iostream>
#include <sstream>
#include <string>

#include "test.hpp"

#include <tao/config.hpp>

namespace tao::config
{
   void unit_test()
   {
      const std::string input = "a = 1 b { c = 42 d = [ null, true ] }";
      const tao::config::value parsed = tao::config::from_string( input, "main" );
      {
         std::ostringstream oss;
         tao::config::to_stream( oss, parsed );
         const std::string string1 = oss.str();
         const std::string reference1 = "{meta:{key:[],position:\"(root):1:1\"},data:{a:{meta:{key:[{key_kind:\"name\",key_data:{index:0,value:\"a\"}}],position:\"main:1:6\"},data:1},b:{meta:{key:[{key_kind:\"name\",key_data:{index:0,value:\"b\"}}],position:\"main:1:9\"},data:{c:{meta:{key:[{key_kind:\"name\",key_data:{index:0,value:\"b\"}},{key_kind:\"name\",key_data:{index:0,value:\"c\"}}],position:\"main:1:17\"},data:42},d:{meta:{key:[{key_kind:\"name\",key_data:{index:0,value:\"b\"}},{key_kind:\"name\",key_data:{index:0,value:\"d\"}}],position:\"main:1:22\"},data:[{meta:{key:[{key_kind:\"name\",key_data:{index:0,value:\"b\"}},{key_kind:\"name\",key_data:{index:0,value:\"d\"}},{key_kind:\"index\",key_data:{index:1,value:0}}],position:\"main:1:24\"},data:null},{meta:{key:[{key_kind:\"name\",key_data:{index:0,value:\"b\"}},{key_kind:\"name\",key_data:{index:0,value:\"d\"}},{key_kind:\"index\",key_data:{index:1,value:1}}],position:\"main:1:30\"},data:true}]}}}}}";
         if( string1 != reference1 ) {
            // LCOV_EXCL_START
            std::cerr << string1 << std::endl;
            std::cerr << reference1 << std::endl;
            std::cerr << "Config to_stream test 1 failed!" << std::endl;
            ++failed;
            // LCOV_EXCL_STOP
         }
      } {
         std::ostringstream oss;
         tao::config::to_stream( oss, parsed, 3 );
         const std::string string2 = oss.str();
         const std::string reference2 = "{\n   meta: {\n      key: [],\n      position: \"(root):1:1\"\n   },\n   data: {\n      a: {\n         meta: {\n            key: [\n               {\n                  key_kind: \"name\",\n                  key_data: {\n                     index: 0,\n                     value: \"a\"\n                  }\n               }\n            ],\n            position: \"main:1:6\"\n         },\n         data: 1\n      },\n      b: {\n         meta: {\n            key: [\n               {\n                  key_kind: \"name\",\n                  key_data: {\n                     index: 0,\n                     value: \"b\"\n                  }\n               }\n            ],\n            position: \"main:1:9\"\n         },\n         data: {\n            c: {\n               meta: {\n                  key: [\n                     {\n                        key_kind: \"name\",\n                        key_data: {\n                           index: 0,\n                           value: \"b\"\n                        }\n                     },\n                     {\n                        key_kind: \"name\",\n                        key_data: {\n                           index: 0,\n                           value: \"c\"\n                        }\n                     }\n                  ],\n                  position: \"main:1:17\"\n               },\n               data: 42\n            },\n            d: {\n               meta: {\n                  key: [\n                     {\n                        key_kind: \"name\",\n                        key_data: {\n                           index: 0,\n                           value: \"b\"\n                        }\n                     },\n                     {\n                        key_kind: \"name\",\n                        key_data: {\n                           index: 0,\n                           value: \"d\"\n                        }\n                     }\n                  ],\n                  position: \"main:1:22\"\n               },\n               data: [\n                  {\n                     meta: {\n                        key: [\n                           {\n                              key_kind: \"name\",\n                              key_data: {\n                                 index: 0,\n                                 value: \"b\"\n                              }\n                           },\n                           {\n                              key_kind: \"name\",\n                              key_data: {\n                                 index: 0,\n                                 value: \"d\"\n                              }\n                           },\n                           {\n                              key_kind: \"index\",\n                              key_data: {\n                                 index: 1,\n                                 value: 0\n                              }\n                           }\n                        ],\n                        position: \"main:1:24\"\n                     },\n                     data: null\n                  },\n                  {\n                     meta: {\n                        key: [\n                           {\n                              key_kind: \"name\",\n                              key_data: {\n                                 index: 0,\n                                 value: \"b\"\n                              }\n                           },\n                           {\n                              key_kind: \"name\",\n                              key_data: {\n                                 index: 0,\n                                 value: \"d\"\n                              }\n                           },\n                           {\n                              key_kind: \"index\",\n                              key_data: {\n                                 index: 1,\n                                 value: 1\n                              }\n                           }\n                        ],\n                        position: \"main:1:30\"\n                     },\n                     data: true\n                  }\n               ]\n            }\n         }\n      }\n   }\n}";
         if( string2 != reference2 ) {
            // LCOV_EXCL_START
            std::cerr << string2.size() << " " << reference2.size() << std::endl;
            std::cerr << string2 << std::endl;
            std::cerr << reference2 << std::endl;
            std::cerr << "Config to_stream test 2 failed!" << std::endl;
            ++failed;
            // LCOV_EXCL_STOP
         }
      }
   }

}  // namespace tao::config

#include "main.hpp"
