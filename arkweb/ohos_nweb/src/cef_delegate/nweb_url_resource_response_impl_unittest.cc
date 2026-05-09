/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#include "ohos_nweb/src/cef_delegate/nweb_url_resource_response_impl.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <memory>
#include <thread>
#include <vector>

namespace OHOS::NWeb {

class NWebUrlResourceResponseImplTest : public ::testing::Test {
 protected:
  void SetUp() override {
    response_ = std::make_shared<NWebUrlResourceResponseImpl>();
  }

  void TearDown() override { response_.reset(); }

  std::shared_ptr<NWebUrlResourceResponseImpl> response_;
};

TEST_F(NWebUrlResourceResponseImplTest, ResponseHeadersThreadSafety) {
  const int kNumIterations = 1000;
  std::vector<std::thread> threads;

  for (int i = 0; i < 4; ++i) {
    threads.emplace_back([this, i]() {
      for (int j = 0; j < kNumIterations; ++j) {
        std::map<std::string, std::string> headers;
        headers["Header-" + std::to_string(i) + "-" + std::to_string(j)] =
            "Value-" + std::to_string(j);
        response_->PutResponseHeaders(headers);

        auto read_headers = response_->ResponseHeaders();
        EXPECT_FALSE(read_headers.empty());
      }
    });
  }

  for (auto& thread : threads) {
    thread.join();
  }
}

}  // namespace OHOS::NWeb
