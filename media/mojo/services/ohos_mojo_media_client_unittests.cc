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

#include "media/base/audio_decoder.h"
#include "media/base/media_log.h"
#include "media/base/ohos/ohos_cdm_factory.h"
#include "media/mojo/mojom/frame_interface_factory.mojom.h"
#include "media/mojo/services/ohos_mojo_media_client.h"
#include "testing/gmock/include/gmock/gmock.h"
#include "testing/gtest/include/gtest/gtest.h"

namespace media {
class MockFrameInterfaceFactory : public mojom::FrameInterfaceFactory {
 public:
  MOCK_METHOD(
      void,
      CreateProvisionFetcher,
      (mojo::PendingReceiver<mojom::ProvisionFetcher> provision_fetcher),
      (override));
  MOCK_METHOD(void,
              CreateCdmStorage,
              (mojo::PendingReceiver<mojom::CdmStorage> cdm_storage),
              (override));
  MOCK_METHOD(bool, GetCdmOrigin, (::url::Origin * out_cdm_origin), (override));
  MOCK_METHOD(void, GetCdmOrigin, (GetCdmOriginCallback callback), (override));
  MOCK_METHOD(void,
              BindEmbedderReceiver,
              (mojo::GenericPendingReceiver receiver),
              (override));
};

class OHOSMojoMediaClientTest : public ::testing::Test {
 protected:
  OHOSMojoMediaClientTest() = default;
  ~OHOSMojoMediaClientTest() override = default;

  void SetUp() override {
    client = std::make_unique<OHOSMojoMediaClient>();
    Mock_frame_interfaces = std::make_unique<MockFrameInterfaceFactory>();
  }

  void TearDown() override {
    client.reset();
    frame_interfaces.reset();
    Mock_frame_interfaces.reset();
  }
  std::unique_ptr<mojom::FrameInterfaceFactory> frame_interfaces;
  std::unique_ptr<OHOSMojoMediaClient> client;
  std::unique_ptr<MockFrameInterfaceFactory> Mock_frame_interfaces;
};

TEST_F(OHOSMojoMediaClientTest, CreateAudioDecoder) {
  scoped_refptr<base::SequencedTaskRunner> task_runner = nullptr;
  std::unique_ptr<MediaLog> media_log = nullptr;
  auto audio_decoder =
      client->CreateAudioDecoder(task_runner, std::move(media_log));
  ASSERT_NE(audio_decoder, nullptr);
}

TEST_F(OHOSMojoMediaClientTest, CreateCdmFactory) {
  frame_interfaces = std::move(Mock_frame_interfaces);
  auto cdm_factory = client->CreateCdmFactory(frame_interfaces.get());
  ASSERT_NE(cdm_factory, nullptr);
}
}  // namespace media
