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

#include "base/test/test_simple_task_runner.h"
#include "media/base/media_drm_storage.h"
#include "media/mojo/mojom/frame_interface_factory.mojom.h"
#include "media/mojo/services/mojo_media_drm_storage.h"
#include "media/mojo/services/ohos_mojo_util.h"
#include "mojo/public/cpp/bindings/pending_remote.h"
#include "testing/gmock/include/gmock/gmock.h"
#include "testing/gtest/include/gtest/gtest.h"
using ::testing::_;
using ::testing::Invoke;

namespace media {
namespace ohos_mojo_util {
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

class OHOSMojoUtilTest : public ::testing::Test {
 protected:
  OHOSMojoUtilTest() = default;
  ~OHOSMojoUtilTest() override = default;

  void SetUp() override {
    Mock_frame_interfaces = std::make_unique<MockFrameInterfaceFactory>();
  }

  void TearDown() override {}
  std::unique_ptr<MockFrameInterfaceFactory> Mock_frame_interfaces;
  std::unique_ptr<mojom::FrameInterfaceFactory> frame_interfaces;
};

TEST_F(OHOSMojoUtilTest, CreateProvisionFetcher) {
  mojo::PendingRemote<mojom::ProvisionFetcher> provision_fetcher;
  EXPECT_CALL(*Mock_frame_interfaces, CreateProvisionFetcher(_))
      .WillOnce(Invoke([&]() {
        auto receiver = provision_fetcher.InitWithNewPipeAndPassReceiver();
      }));
  scoped_refptr<base::SingleThreadTaskRunner> task_runner(
      base::MakeRefCounted<base::TestSimpleTaskRunner>());
  base::SingleThreadTaskRunner::CurrentDefaultHandle sttcd(task_runner);
  frame_interfaces = std::move(Mock_frame_interfaces);
  auto result = ohos_mojo_util::CreateProvisionFetcher(frame_interfaces.get());
  ASSERT_TRUE(provision_fetcher.is_valid());
}

TEST_F(OHOSMojoUtilTest, CreateMediaDrmStorage) {
  mojo::PendingRemote<mojom::MediaDrmStorage> media_drm_storage;
  EXPECT_CALL(*Mock_frame_interfaces, BindEmbedderReceiver(_))
      .WillOnce(Invoke([&](mojo::GenericPendingReceiver receiver) {
        auto receiver_ = media_drm_storage.InitWithNewPipeAndPassReceiver();
      }));
  scoped_refptr<base::SingleThreadTaskRunner> task_runner(
      base::MakeRefCounted<base::TestSimpleTaskRunner>());
  base::SingleThreadTaskRunner::CurrentDefaultHandle sttcd(task_runner);
  frame_interfaces = std::move(Mock_frame_interfaces);
  auto result = ohos_mojo_util::CreateMediaDrmStorage(frame_interfaces.get());
  EXPECT_NE(result, nullptr);
  ASSERT_TRUE(media_drm_storage.is_valid());
}
}  // namespace ohos_mojo_util
}  // namespace media
