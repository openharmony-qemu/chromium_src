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

#include "base/test/task_environment.h"
#include "testing/gmock/include/gmock/gmock.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "third_party/blink/public/platform/resource_load_info_notifier_wrapper.h"
#include "third_party/blink/renderer/platform/loader/fetch/url_loader/resource_request_client.h"
#define private public
#include "third_party/blink/renderer/platform/loader/fetch/url_loader/resource_request_sender.h"
#undef private
#include "services/network/public/mojom/url_loader.mojom.h"
#include "services/network/public/mojom/url_response_head.mojom.h"
#include "third_party/bounds_checking_function/include/securec.h"

using testing::_;

namespace blink {

class MockResourceRequestClient : public ResourceRequestClient {
 public:
  MockResourceRequestClient() = default;
  ~MockResourceRequestClient() override = default;

  MOCK_METHOD2(OnUploadProgress, void(uint64_t position, uint64_t size));

  using FollowRedirectCallback =
      base::OnceCallback<void(std::vector<std::string> removed_headers,
                              net::HttpRequestHeaders modified_headers)>;
  MOCK_METHOD3(OnReceivedRedirect,
               void(const net::RedirectInfo& redirect_info,
                    network::mojom::URLResponseHeadPtr head,
                    FollowRedirectCallback follow_redirect_callback));

  MOCK_METHOD3(OnReceivedResponse,
               void(network::mojom::URLResponseHeadPtr head,
                    mojo::ScopedDataPipeConsumerHandle body,
                    std::optional<mojo_base::BigBuffer> cached_metadata));

#if BUILDFLAG(ARKWEB_RESOURCE_INTERCEPTION)
  MOCK_METHOD2(OnTransferDataWithSharedMemory,
               void(base::ReadOnlySharedMemoryRegion region,
                    uint64_t buffer_size));
#endif

  MOCK_METHOD1(OnTransferSizeUpdated, void(int transfer_size_diff));
  MOCK_METHOD1(OnCompletedRequest,
               void(const network::URLLoaderCompletionStatus& status));
};

class ResourceRequestSenderExtTest : public ::testing::Test {
 public:
  void SetUp() override {
    task_runner_ = base::SingleThreadTaskRunner::GetCurrentDefault();
    sender_ = std::make_unique<ResourceRequestSender>();
  }

  void TearDown() override { sender_.reset(); }

  base::ReadOnlySharedMemoryRegion CreateTestSharedMemoryRegion(
      const std::string& data) {
    auto writable_region =
        base::WritableSharedMemoryRegion::Create(data.size());
    auto mapping = writable_region.Map();
    if (memcpy_s(mapping.memory(), mapping.size(), data.c_str(), data.size()) !=
        0) {
      return base::ReadOnlySharedMemoryRegion();
    }
    return base::WritableSharedMemoryRegion::ConvertToReadOnly(
        std::move(writable_region));
  }

  scoped_refptr<base::SingleThreadTaskRunner> task_runner_;
  base::test::TaskEnvironment task_environment_;
  std::unique_ptr<ResourceRequestSender> sender_;
};

#if BUILDFLAG(ARKWEB_RESOURCE_INTERCEPTION)
TEST_F(ResourceRequestSenderExtTest,
       OnTransferDataWithSharedMemory_NullRequestInfo) {
  auto region = CreateTestSharedMemoryRegion("test data");
  uint64_t buffer_size = 9;
  testing::internal::CaptureStderr();
  sender_->OnTransferDataWithSharedMemory(std::move(region), buffer_size);
  std::string log_output = testing::internal::GetCapturedStderr();
  EXPECT_EQ(log_output.find("OnTransferDataWithSharedMemory"),
            std::string::npos);
}

TEST_F(ResourceRequestSenderExtTest,
       OnTransferDataWithSharedMemory_NullNotifierWrapper) {
  auto region = CreateTestSharedMemoryRegion("test data");
  uint64_t buffer_size = 9;

  auto mock_client = base::MakeRefCounted<MockResourceRequestClient>();
  sender_->request_info_ =
      std::make_unique<ResourceRequestSender::PendingRequestInfo>(
          mock_client, network::mojom::RequestDestination::kDocument,
          KURL("https://example.com"), nullptr);

  EXPECT_CALL(*mock_client, OnTransferDataWithSharedMemory(_, buffer_size))
      .Times(1);
  testing::internal::CaptureStderr();
  sender_->OnTransferDataWithSharedMemory(std::move(region), buffer_size);
  std::string log_output = testing::internal::GetCapturedStderr();
  EXPECT_NE(log_output.find("notifier wrapper is nullptr"), std::string::npos);
}

TEST_F(ResourceRequestSenderExtTest,
       OnTransferDataWithSharedMemory_ValidNotifierWrapper) {
  auto region = CreateTestSharedMemoryRegion("test data");
  uint64_t buffer_size = 9;

  auto mock_client = base::MakeRefCounted<MockResourceRequestClient>();
  auto notifier = std::make_unique<ResourceLoadInfoNotifierWrapper>(
      base::WeakPtr<WeakWrapperResourceLoadInfoNotifier>(nullptr));

  sender_->request_info_ =
      std::make_unique<ResourceRequestSender::PendingRequestInfo>(
          mock_client, network::mojom::RequestDestination::kDocument,
          KURL("https://example.com"), std::move(notifier));

  EXPECT_CALL(*mock_client, OnTransferDataWithSharedMemory(_, buffer_size))
      .Times(1);
  testing::internal::CaptureStderr();
  sender_->OnTransferDataWithSharedMemory(std::move(region), buffer_size);
  std::string log_output = testing::internal::GetCapturedStderr();
  EXPECT_EQ(log_output.find("notifier wrapper is nullptr"), std::string::npos);
}
#endif  // BUILDFLAG(ARKWEB_RESOURCE_INTERCEPTION)

}  // namespace blink