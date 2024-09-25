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

#include "testing/gmock/include/gmock/gmock.h"
#include "testing/gtest/include/gtest/gtest.h"
#define private public
#include "components/viz/service/display_embedder/output_surface_provider_impl.h"
#undef private
#include "cef/libcef/browser/osr/software_output_device_proxy.h"
#include "mojo/public/cpp/bindings/pending_remote.h"
#include "mojo/public/cpp/bindings/receiver.h"
#include "mojo/public/cpp/bindings/remote.h"
#include "mojo/public/cpp/bindings/sync_call_restrictions.h"

namespace viz {

using ::testing::_;

class MockDisplayClient : public viz::mojom::DisplayClient {
 public:
  MOCK_METHOD(void,
              UseProxyOutputDevice,
              (UseProxyOutputDeviceCallback callback),
              (override));
  MOCK_METHOD(
      void,
      CreateLayeredWindowUpdater,
      (mojo::PendingReceiver<viz::mojom::LayeredWindowUpdater> receiver),
      (override));
  MOCK_METHOD(void,
              DidCompleteSwapWithNewSizeOHOS,
              (const gfx::Size& size),
              (override));
};

class OutputSurfaceProviderImplTest : public testing::Test {
 protected:
  void SetUp() override {
    display_client_ = std::make_unique<MockDisplayClient>();
    display_client_receiver_ =
        std::make_unique<mojo::Receiver<viz::mojom::DisplayClient>>(
            display_client_.get());
    display_client_remote_.Bind(
        display_client_receiver_->BindNewPipeAndPassRemote());
  }

  std::unique_ptr<MockDisplayClient> display_client_;
  std::unique_ptr<mojo::Receiver<viz::mojom::DisplayClient>>
      display_client_receiver_;
  mojo::Remote<mojom::DisplayClient> display_client_remote_;
};

TEST_F(OutputSurfaceProviderImplTest,
       CreateSoftwareOutputDeviceForPlatform_AllTrue) {
  OutputSurfaceProviderImpl provider(false);
  mojo::ScopedAllowSyncCallForTesting allow_sync;
  EXPECT_CALL(*display_client_, UseProxyOutputDevice(_))
      .Times(testing::AtLeast(0));
  auto software_output_device = provider.CreateSoftwareOutputDeviceForPlatform(
      gpu::kNullSurfaceHandle, display_client_remote_.get());
  ASSERT_TRUE(software_output_device);
  EXPECT_TRUE(
      static_cast<SoftwareOutputDeviceProxy*>(software_output_device.get()));
}
}  // namespace viz
