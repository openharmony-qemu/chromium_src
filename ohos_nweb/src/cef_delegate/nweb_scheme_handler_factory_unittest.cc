/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <map>
#include <string>


#include "ohos_nweb/src/capi/arkweb_scheme_handler.h"
#include "ohos_nweb/src/ndk/scheme_handler/resource_request.h"
#define private public
#include "ohos_nweb/src/cef_delegate/nweb_scheme_handler_factory.h"
#include "ohos_nweb/src/ndk/scheme_handler/resource_handler.h"


namespace {
typedef std::map<std::string, CefRefPtr<OHOS::NWeb::NWebSchemeHandlerFactory>>
    SchemeHandlerFactoryMap;
SchemeHandlerFactoryMap g_scheme_handler_factory_map;

}  // namespace

namespace OHOS::NWeb {
class NWebSchemeHandlerFactoryTest : public ::testing::Test {
 protected:
  void SetUp() override {
    g_scheme_handler_factory_map.clear();
    factory = new NWebSchemeHandlerFactory();
  }

  void TearDown() override {
    g_scheme_handler_factory_map.clear();
    delete factory;
  }

  NWebSchemeHandlerFactory* factory;
};

TEST_F(NWebSchemeHandlerFactoryTest, GetOrCreateForScheme) {
  std::string scheme = "testScheme";
  CefRefPtr<NWebSchemeHandlerFactory> factory_ =
      NWebSchemeHandlerFactory::GetOrCreateForScheme(scheme);
  ASSERT_NE(factory_, nullptr);
}

TEST_F(NWebSchemeHandlerFactoryTest, ClearSchemeHandlers) {
  std::string web_tag = "test_web_tag";
  NWebSchemeHandlerFactory::ClearSchemeHandlers(web_tag);
  ASSERT_NE(factory, nullptr);
}

TEST_F(NWebSchemeHandlerFactoryTest, ClearServiceWorkerSchemeHandler) {
  NWebSchemeHandlerFactory::ClearServiceWorkerSchemeHandler();
}

TEST_F(NWebSchemeHandlerFactoryTest, Create_ServiceWorker) {
  std::string scheme = "testScheme";
  CefRefPtr<CefBrowser> browser;
  CefRefPtr<CefRequest> request;
  factory->Create(browser, nullptr, "http", request);
  ASSERT_NE(factory, nullptr);
}

TEST_F(NWebSchemeHandlerFactoryTest, SetSchemeHandlerForSW) {
  std::string scheme = "testScheme";
  factory->SetSchemeHandlerForSW(nullptr);
  ASSERT_NE(factory, nullptr);
}

TEST_F(NWebSchemeHandlerFactoryTest, SetSchemeHandler) {
  std::string web_tag = "test_tag";
  factory->SetSchemeHandler(web_tag, nullptr);
  ASSERT_NE(factory, nullptr);
}

TEST_F(NWebSchemeHandlerFactoryTest, RemoveSchemeHandler) {
  factory->SetSchemeHandler("testTag", new ArkWeb_SchemeHandler());
  std::string web_tag = "testTag";
  EXPECT_NE(factory->FromTag(web_tag), nullptr);

  factory->RemoveSchemeHandler(web_tag);
  EXPECT_EQ(factory->FromTag(web_tag), nullptr);
}

TEST_F(NWebSchemeHandlerFactoryTest, RemoveServiceWorkerSchemeHandler) {
  ArkWeb_SchemeHandler* scheme_handler_for_sw = new ArkWeb_SchemeHandler();
  factory->SetSchemeHandlerForSW(scheme_handler_for_sw);
  EXPECT_NE(factory->FromTag(""), scheme_handler_for_sw);

  factory->RemoveServiceWorkerSchemeHandler();
  EXPECT_EQ(factory->FromTag(""), nullptr);
}

TEST_F(NWebSchemeHandlerFactoryTest, OnRequestStop) {
  CefRefPtr<CefRequest> cef_request;
  ArkWeb_ResourceRequest* request_ = new ArkWeb_ResourceRequest(cef_request);
  std::string web_tag = "test_tag";

  factory->SetSchemeHandler(web_tag, new ArkWeb_SchemeHandler());
  factory->OnRequestStop(request_, web_tag, false);
  ASSERT_NE(factory, nullptr);
}
}  // namespace OHOS::NWeb
