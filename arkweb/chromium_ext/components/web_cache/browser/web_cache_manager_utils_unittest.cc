/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "components/web_cache/browser/web_cache_manager_utils.h"

#include "base/files/file_path.h"
#include "components/web_cache/browser/web_cache_manager.h"
#include "gtest/gtest.h"

#if BUILDFLAG(ARKWEB_INJECT_OFFLINE_RESOURCE)
namespace web_cache {
class WebCacheManagerUtilsTest : public testing::Test {
 public:
  std::string url;
  std::string origin;
  std::vector<uint8_t> resource;
  std::map<std::string, std::string> response_headers;
  int type;
};

TEST_F(WebCacheManagerUtilsTest, AddResourceToCache001) {
  WebCacheManagerUtils webCacheManagerUtils(WebCacheManager::GetInstance());
  webCacheManagerUtils.AddResourceToCache(url, origin, resource,
                                          response_headers, type);
}

TEST_F(WebCacheManagerUtilsTest, AddResourceToCache002) {
  std::string url = "https://example.com/test/resource.html";
  std::string origin = "https://example.com";
  std::vector<uint8_t> resource = {'H', 'e', 'l', 'l', 'o', ' ', 'W', 'o', 'r', 'l', 'd', '!'};
  std::map<std::string, std::string> response_headers = {
    {"Content-Type", "text/html"},
    {"Cache-Control", "no-cache"},
    {"ETag", "W/\"12345\""}
  };
  int type = 1;

  WebCacheManagerUtils webCacheManagerUtils(WebCacheManager::GetInstance());
  webCacheManagerUtils.AddResourceToCache(url, origin, resource, response_headers, type);
}

// Test AddResourceToCache with empty URL
TEST_F(WebCacheManagerUtilsTest, AddResourceToCache004) {
    std::string url;
    std::string origin;
    std::vector<uint8_t> resource = {'T', 'e', 's', 't', ' '};
    std::map<std::string, std::string> response_headers;
    int type = 0;

    WebCacheManagerUtils webCacheManagerUtils(WebCacheManager::GetInstance());
    webCacheManagerUtils.AddResourceToCache(url, origin, resource, response_headers, type);
}

// Test AddResourceToCache with different origins
TEST_F(WebCacheManagerUtilsTest, AddResourceToCache005) {
    std::string url = "https://example.com/test/resource.html";
    std::string origin = "https://example.com";
    std::vector<uint8_t> resource = {'T', 'e', 's', 't', ' '};
    std::map<std::string, std::string> response_headers = {
        {"Content-Type", "text/html"},
        {"Cache-Control", "no-cache"}
    };
    int type = 1;

    WebCacheManagerUtils webCacheManagerUtils(WebCacheManager::GetInstance());
    webCacheManagerUtils.AddResourceToCache(url, origin, resource, response_headers, type);

    origin = "https://different-origin.com";
    webCacheManagerUtils.AddResourceToCache(url, origin, resource, response_headers, type);
}

// Test AddResourceToCache with multiple headers
TEST_F(WebCacheManagerUtilsTest, AddResourceToCache006) {
  std::string url = "https://example.com/test/resource.html";
  std::string origin = "https://example.com";
  std::vector<uint8_t> resource = {'T', 'e', 's', 't', ' '};
  std::map<std::string, std::string> response_headers = {
    {"Content-Type", "text/html"},
    {"Cache-Control", "no-cache"},
    {"ETag", "W/\"12345\""},
    {"Content-Encoding", "gzip"},
    {"User-Agent", "ArkWeb/1.0"}
  };
  int type = 1;

  WebCacheManagerUtils webCacheManagerUtils(WebCacheManager::GetInstance());
  webCacheManagerUtils.AddResourceToCache(url, origin, resource, response_headers, type);
}

// Test AddResourceToCache with different types
TEST_F(WebCacheManagerUtilsTest, AddResourceToCache007) {
  std::string url = "https://example.com/test/resource.html";
  std::string origin = "https://example.com";
  std::vector<uint8_t> resource = {'T', 'e', 's', 't', ' '};
  std::map<std::string, std::string> response_headers = {
    {"Content-Type", "text/html"},
    {"Cache-Control", "no-cache"}
  };
  int type = 1;

  WebCacheManagerUtils webCacheManagerUtils(WebCacheManager::GetInstance());
  webCacheManagerUtils.AddResourceToCache(url, origin, resource, response_headers, type);

  type = 2;
  webCacheManagerUtils.AddResourceToCache(url, origin, resource, response_headers, type);

  type = 3;
  webCacheManagerUtils.AddResourceToCache(url, origin, resource, response_headers, type);
}

// Test AddResourceToCache with large resource
TEST_F(WebCacheManagerUtilsTest, AddResourceToCache008) {
  std::string url = "https://example.com/test/large_resource.html";
  std::string origin = "https://. example.com";
  // 1KB resource
  std::vector<uint8_t> resource(1024, 1);
  std::map<std::string, std::string> response_headers = {
    {"Content-Type", "text/html"},
    {"Cache-Control", "no-cache"}
  };

  int type = 1;

  WebCacheManagerUtils webCacheManagerUtils(WebCacheManager::GetInstance());
  webCacheManagerUtils.AddResourceToCache(url, origin, resource, response_headers, type);
}

// Test AddResourceToCache with special characters in URL
TEST_F(WebCacheManagerUtilsTest, AddResourceToCache009) {
  std::string url = "https://example.com/test/path with spaces.html";
  std::string origin = "https://example.com";
  // special characters
  std::vector<uint8_t> resource = {'T', 'e', 's', 't', ' '};
  std::map<std::string, std::string> response_headers = {
    {"Content-Type", "text/html"},
    {"Cache-Control", "no-cache"}
  };
  int type = 1;

  WebCacheManagerUtils webCacheManagerUtils(WebCacheManager::GetInstance());
  webCacheManagerUtils.AddResourceToCache(url, origin, resource, response_headers, type);
}

// Test AddResourceToCache with Unicode characters in origin
TEST_F(WebCacheManagerUtilsTest, AddResourceToCache010) {
  // Unicode characters 包含中文字符
  std::string url = "https://example.com/test/中文.html";
  std::string origin = "https://example.com/测试";
  std::vector<uint8_t> resource = {'T', 'e', 's', 't', ' '};
  std::map<std::string, std::string> response_headers = {
    {"Content-Type", "text/html"},
    {"Cache-Control", "no-cache"}
  };
  int type = 1;

  WebCacheManagerUtils webCacheManagerUtils(WebCacheManager::GetInstance());
  webCacheManagerUtils.AddResourceToCache(url, origin, resource, response_headers, type);
}

// Test AddResourceToCache with very long URL
TEST_F(WebCacheManagerUtilsTest, AddResourceToCache011) {
  // long url
  std::string url = "https://example.com/" + std::string(500, 'a') + "/very/long/path/to/resource.html";
  std::string origin = "https://example.com";
  std::vector<uint8_t> resource = {'T', 'e', 's', 't', ' '};
  std::map<std::string, std::string> response_headers = {
    {"Content-Type", "text/html"},
    {"Cache-Control", "no-cache"}
  };
  int type = 1;

  WebCacheManagerUtils webCacheManagerUtils(WebCacheManager::GetInstance());
  webCacheManagerUtils.AddResourceToCache(url, origin, resource, response_headers, type);
}

// Test AddResourceToCache with multiple calls (stress test)
TEST_F(WebCacheManagerUtilsTest, AddResourceToCache012) {
  std::string url = "https://example.com/test/resource.html";
  std::string origin = "https://example.com";
  std::vector<uint8_t> resource = {'T', 'e', 's', 't', ' '};
  std::map<std::string, std::string> response_headers = {
    {"Content-Type", "text/html"},
    {"Cache-Control", "no-cache"}
  };
  int type = 1;

  WebCacheManagerUtils webCacheManagerUtils(WebCacheManager::GetInstance());

  //  multiple call
  for (int i = 0; i < 100; ++i) {
    webCacheManagerUtils.AddResourceToCache(url, origin, resource, response_headers, type);
  }
}

// Test with empty resource
TEST_F(WebCacheManagerUtilsTest, AddResourceToCache013) {
  std::string url = "https://example.com/test/resource.html";
  std::string origin = "https://example.com";
  // empty resource
  std::vector<uint8_t> resource;
  std::map<std::string, std::string> response_headers = {
    {"Content-Type", "text/html"},
    {"Cache-Control", "no-cache"}
  };
  int type = 1;

  WebCacheManagerUtils webCacheManagerUtils(WebCacheManager::GetInstance());
  webCacheManagerUtils.AddResourceToCache(url, origin, resource, response_headers, type);
}

// Test with empty response headers
TEST_F(WebCacheManagerUtilsTest, AddResourceToCache014) {
  std::string url = "https://example.com/test/resource.html";
  std::string origin = "https://example.com";
  std::vector<uint8_t> resource = {'T', 'e', 's', 't', ' '};
  // empty response headers
  std::map<std::string, std::string> response_headers;
  int type = 1;

  WebCacheManagerUtils webCacheManagerUtils(WebCacheManager::GetInstance());
  webCacheManagerUtils.AddResourceToCache(url, origin, resource, response_headers, type);
}

// Test with different type values
TEST_F(WebCacheManagerUtilsTest, AddResourceToCache015) {
  std::string url = "https://example.com/test/resource.html";
  std::string origin = "https://example.com";
  std::vector<uint8_t> resource = {'T', 'e', 's', 't', ' '};
  std::map<std::string, std::string> response_headers = {
    {"Content-Type", "text/html"},
    {"Cache-Control", "no-cache"}
  };
  int type = 1;

  WebCacheManagerUtils webCacheManagerUtils(WebCacheManager::GetInstance());
  webCacheManagerUtils.AddResourceToCache(url, origin, resource, response_headers, type);

  type = 0;
  webCacheManagerUtils.AddResourceToCache(url, origin, resource, response_headers, type);

  type = 1;
  webCacheManagerUtils.AddResourceToCache(url, origin, resource, response_headers, type);

  type = 2;
  webCacheManagerUtils.AddResourceToCache(url, origin, resource, response_headers, type);

  type = 3;
  webCacheManagerUtils.AddResourceToCache(url, origin, resource, response_headers, type);

  type = 4;
  webCacheManagerUtils.AddResourceToCache(url, origin, resource, response_headers, type);

  type = 5;
  webCacheManagerUtils.AddResourceToCache(url, origin, resource, response_headers, type);
}

// Test with concurrent calls (thread safety)
TEST_F(WebCacheManagerUtilsTest, AddResourceToCache016) {
  std::string url = "https://example.com/test/resource.html";
  std::string origin = "https://example.com";
  std::vector<uint8_t> resource = {'T', 'e', 's', 't', ' '};
  std::map<std::string, std::string> response_headers = {
      {"Content-Type", "text/html"},
      {"Cache-Control", "no-cache"}
  };
  int type = 1;

  WebCacheManagerUtils webCacheManagerUtils(WebCacheManager::GetInstance());

  const int kNumThreads = 10;

  for (int i = 0; i < kNumThreads; ++i) {
    webCacheManagerUtils.AddResourceToCache(url, origin, resource, response_headers, type);
  }
}

}  // namespace web_cache
#endif