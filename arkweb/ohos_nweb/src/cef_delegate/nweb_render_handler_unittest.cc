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

#include "nweb_render_handler.h"

#include <gtest/gtest.h>

#include <memory>

namespace OHOS::NWeb {
namespace {
std::shared_ptr<NWebRenderHandler> g_nweb_render_handler = nullptr;
}

class NWebRenderHandlerTest : public ::testing::Test {
 public:
  static void SetUpTestCase(void);
  static void TearDownTestCase(void);
  void SetUp();
  void TearDown();
};

void NWebRenderHandlerTest::SetUpTestCase(void) {}

void NWebRenderHandlerTest::TearDownTestCase(void) {}

void NWebRenderHandlerTest::SetUp() {
  g_nweb_render_handler = std::make_shared<NWebRenderHandler>();
  ASSERT_NE(g_nweb_render_handler, nullptr);
}

void NWebRenderHandlerTest::TearDown(void) {
  g_nweb_render_handler = nullptr;
}

#if BUILDFLAG(IS_OHOS)
TEST_F(NWebRenderHandlerTest, OnNativeEmbedLifecycleChange) {
  CefRefPtr<CefBrowser> browser = nullptr;
  CefRenderHandler::CefNativeEmbedData data_info;
  data_info.status = CREATE;
  data_info.surfaceId = "";
  data_info.embedId = "";
  data_info.info.id = "";
  data_info.info.x = 0;
  data_info.info.y = 0;
  data_info.info.width = 0;
  data_info.info.height = 0;
  data_info.info.type = "";
  data_info.info.src = "";
  data_info.info.url = "";
  data_info.info.tag = "";
  data_info.info.params = {{"key1", "value1"}, {"key2", "value2"}};
  g_nweb_render_handler->OnNativeEmbedLifecycleChange(browser, data_info);
}

TEST_F(NWebRenderHandlerTest, OnNativeEmbedGestureEvent) {
  cef_embed_touch_event_t touchEvent;

  g_nweb_render_handler->OnNativeEmbedGestureEvent(nullptr, touchEvent,
                                                   nullptr);
}
#endif
}  // namespace OHOS::NWeb
