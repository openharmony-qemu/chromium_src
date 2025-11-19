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

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "content/public/browser/message_port_provider.h"
#include "content/public/browser/web_contents.h"
#include "content/public/test/mock_render_process_host.h"
#include "content/public/test/test_browser_context.h"
#include "content/public/test/test_renderer_host.h"
#include "third_party/blink/public/common/messaging/web_message_port.h"

using namespace std;
namespace content {

class MessagePortProviderForIncludeTest : public RenderViewHostTestHarness {
 protected:
  MessagePortProviderForIncludeTest()
      : RenderViewHostTestHarness(
            base::test::TaskEnvironment::TimeSource::MOCK_TIME) {}

 public:
  void SetUp() override { RenderViewHostTestHarness::SetUp(); }
  void TearDown() override { RenderViewHostTestHarness::TearDown(); }
};
#if BUILDFLAG(IS_ARKWEB)
TEST_F(MessagePortProviderForIncludeTest,
    OhosPostMessageToFrameNormalExecution) {
  std::u16string source_origin = u"https://source.example.com";
  std::u16string target_origin = u"https://target.example.com";
  std::u16string data = u"test message data";

  std::vector<blink::WebMessagePort> empty_ports;
  Page& page = web_contents()->GetPrimaryPage();
  EXPECT_NO_FATAL_FAILURE(MessagePortProvider::OhosPostMessageToFrame(
      page, source_origin, target_origin, data, empty_ports));

  std::vector<blink::WebMessagePort> single_port;
  auto port_pair = blink::WebMessagePort::CreatePair();
  single_port.push_back(std::move(port_pair.first));
  EXPECT_NO_FATAL_FAILURE(MessagePortProvider::OhosPostMessageToFrame(
      page, source_origin, target_origin, data, single_port));
}

TEST_F(MessagePortProviderForIncludeTest,
    OhosPostMessageToFrameMultiplePortsLoop) {
  std::u16string source_origin = u"https://source.example.com";
  std::u16string target_origin = u"https://target.example.com";
  std::u16string data = u"test message with multiple ports";
  const size_t port_count = 5;
  std::vector<blink::WebMessagePort> multiple_ports;
  for (size_t i = 0; i < port_count; ++i) {
    auto port_pair = blink::WebMessagePort::CreatePair();
    multiple_ports.push_back(std::move(port_pair.first));
  }
  Page& page = web_contents()->GetPrimaryPage();
  EXPECT_NO_FATAL_FAILURE(MessagePortProvider::OhosPostMessageToFrame(
      page, source_origin, target_origin, data, multiple_ports));
}
#endif
}  // namespace content