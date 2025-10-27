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

#include "cef/include/cef_browser.h"
#define private public
#include "ohos_nweb/src/cef_delegate/nweb_access_request_delegate.h"
#undef private

namespace OHOS::NWeb {
class MockCefAccessRequest : public CefAccessRequest {
 public:
  CefString Origin() override { return "hello"; }
  int ResourceAcessId() override { return 1; }
  void ReportRequestResult(bool allowed) override {}
  void AddRef() const override {}
  bool Release() const override { return false; }
  bool HasOneRef() const override { return false; }
  bool HasAtLeastOneRef() const override { return false; }
};

class MockCefScreenCaptureAccessRequest : public CefScreenCaptureAccessRequest {
 public:
  CefString Origin() override { return "hello"; }
  void SetCaptureMode(int32_t mode) override { mode_ = mode; }
  void SetCaptureSourceId(int32_t sourceId) override {}
  void ReportRequestResult(bool allowed) override {}
  void AddRef() const override {}
  bool Release() const override { return false; }
  bool HasOneRef() const override { return false; }
  bool HasAtLeastOneRef() const override { return false; }
  int32_t mode_ = -1;
};

class MockNWebScreenCaptureConfig : public NWebScreenCaptureConfig {
 public:
  int32_t GetMode() override { return 1; }
  int32_t GetSourceId() override { return 1; }
};

class NWebAccessRequestDelegateTest : public ::testing::Test {
 protected:
  void SetUp() {}
  void TearDown() {}
};

class NWebScreenCaptureAccessRequestDelegateTest : public ::testing::Test {
 protected:
  void SetUp() {}
  void TearDown() {}
};

TEST_F(NWebAccessRequestDelegateTest, Origin_001) {
  std::shared_ptr<NWebAccessRequestDelegate> access_request_ =
      std::make_shared<NWebAccessRequestDelegate>(nullptr);
  auto result = access_request_->Origin();
  EXPECT_EQ(result, "");
}

TEST_F(NWebAccessRequestDelegateTest, Origin_002) {
  CefRefPtr<MockCefAccessRequest> request = new MockCefAccessRequest();
  std::shared_ptr<NWebAccessRequestDelegate> access_request_ =
      std::make_shared<NWebAccessRequestDelegate>(request);
  auto result = access_request_->Origin();
  EXPECT_NE(result, "");
}

TEST_F(NWebAccessRequestDelegateTest, ResourceAcessId_001) {
  std::shared_ptr<NWebAccessRequestDelegate> access_request_ =
      std::make_shared<NWebAccessRequestDelegate>(nullptr);
  auto result = access_request_->ResourceAcessId();
  EXPECT_EQ(result, -1);
}

TEST_F(NWebAccessRequestDelegateTest, ResourceAcessId_002) {
  CefRefPtr<MockCefAccessRequest> request = new MockCefAccessRequest();
  std::shared_ptr<NWebAccessRequestDelegate> access_request_ =
      std::make_shared<NWebAccessRequestDelegate>(request);
  auto result = access_request_->ResourceAcessId();
  EXPECT_EQ(result, 1);
}

TEST_F(NWebAccessRequestDelegateTest, Agree_001) {
  std::shared_ptr<NWebAccessRequestDelegate> access_request_ =
      std::make_shared<NWebAccessRequestDelegate>(nullptr);
  access_request_->Agree(1);
  EXPECT_EQ(access_request_->request_, nullptr);
}

TEST_F(NWebAccessRequestDelegateTest, Agree_002) {
  CefRefPtr<MockCefAccessRequest> request = new MockCefAccessRequest();
  std::shared_ptr<NWebAccessRequestDelegate> access_request_ =
      std::make_shared<NWebAccessRequestDelegate>(request);
  access_request_->Agree(1);
  EXPECT_NE(access_request_->request_, nullptr);
}

TEST_F(NWebAccessRequestDelegateTest, Agree_003) {
  CefRefPtr<MockCefAccessRequest> request = new MockCefAccessRequest();
  std::shared_ptr<NWebAccessRequestDelegate> access_request_ =
      std::make_shared<NWebAccessRequestDelegate>(request);
  access_request_->Agree(0);
  EXPECT_NE(access_request_->request_, nullptr);
}

TEST_F(NWebAccessRequestDelegateTest, Refuse_001) {
  std::shared_ptr<NWebAccessRequestDelegate> access_request_ =
      std::make_shared<NWebAccessRequestDelegate>(nullptr);
  access_request_->Refuse();
  EXPECT_EQ(access_request_->request_, nullptr);
}

TEST_F(NWebAccessRequestDelegateTest, Refuse_002) {
  CefRefPtr<MockCefAccessRequest> request = new MockCefAccessRequest();
  std::shared_ptr<NWebAccessRequestDelegate> access_request_ =
      std::make_shared<NWebAccessRequestDelegate>(request);
  access_request_->Refuse();
  EXPECT_NE(access_request_->request_, nullptr);
}

TEST_F(NWebScreenCaptureAccessRequestDelegateTest, Origin) {
  CefRefPtr<MockCefScreenCaptureAccessRequest> request =
      new MockCefScreenCaptureAccessRequest();
  std::shared_ptr<NWebScreenCaptureAccessRequestDelegate> access_request_ =
      std::make_shared<NWebScreenCaptureAccessRequestDelegate>(nullptr);
  auto result = access_request_->Origin();
  EXPECT_EQ(result, "");

  access_request_ =
      std::make_shared<NWebScreenCaptureAccessRequestDelegate>(request);
  result = access_request_->Origin();
  EXPECT_NE(result, "");
}

TEST_F(NWebScreenCaptureAccessRequestDelegateTest, Agree) {
  CefRefPtr<MockCefScreenCaptureAccessRequest> request =
      new MockCefScreenCaptureAccessRequest();
  std::shared_ptr<NWebScreenCaptureAccessRequestDelegate> access_request_ =
      std::make_shared<NWebScreenCaptureAccessRequestDelegate>(nullptr);
  std::shared_ptr<MockNWebScreenCaptureConfig> screen_capture_ =
      std::make_shared<MockNWebScreenCaptureConfig>();
  access_request_->Agree(nullptr);
  EXPECT_EQ(request->mode_, -1);

  access_request_ =
      std::make_shared<NWebScreenCaptureAccessRequestDelegate>(request);
  access_request_->Agree(nullptr);
  EXPECT_EQ(request->mode_, 0);

  access_request_->Agree(screen_capture_);
  EXPECT_EQ(request->mode_, 1);
}

TEST_F(NWebScreenCaptureAccessRequestDelegateTest, Refuse) {
  CefRefPtr<MockCefScreenCaptureAccessRequest> request =
      new MockCefScreenCaptureAccessRequest();
  std::shared_ptr<NWebScreenCaptureAccessRequestDelegate> access_request_ =
      std::make_shared<NWebScreenCaptureAccessRequestDelegate>(nullptr);
  access_request_->Refuse();
  EXPECT_EQ(access_request_->request_, nullptr);

  access_request_ =
      std::make_shared<NWebScreenCaptureAccessRequestDelegate>(request);
  access_request_->Refuse();
  EXPECT_NE(access_request_->request_, nullptr);
}
}  // namespace OHOS::NWeb
