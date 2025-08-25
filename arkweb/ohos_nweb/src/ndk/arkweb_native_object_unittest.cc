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

#include <gtest/gtest.h>

#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>

#include "nweb_impl.h"
#define private public
#include "arkweb_native_object.h"
#undef private

namespace OHOS::NWeb {

class ArkWebNativeObjectTest : public ::testing::Test {
 public:
  void SetUp(void);
  void TearDown(void);

  std::weak_ptr<NWebImpl> nweb_weak_;
  int32_t id_ = 0;
  const char* web_tag_ = nullptr;
  std::shared_ptr<NWebImpl> nweb_impl_;
  std::shared_ptr<ArkWebNativeObject> web_object_;
};

void ArkWebNativeObjectTest::SetUp() {
  web_object_ = std::make_shared<ArkWebNativeObject>("test_tag");
  nweb_impl_ = std::make_shared<NWebImpl>(id_++);
  ASSERT_NE(nweb_impl_, nullptr);
}

void ArkWebNativeObjectTest::TearDown() {
  web_object_.reset();
  nweb_impl_ = nullptr;
}

TEST_F(ArkWebNativeObjectTest, BindWebTagToWebpInstanceTest) {
  id_ = 1;
  web_tag_ = "test_tag";
  web_object_->BindWebTagToWebInstance(id_, web_tag_);
  web_object_->BindWebTagToWebInstance(id_, web_tag_);
  EXPECT_NE(ArkWebNativeObject::GetWebInstanceByWebTag(web_tag_), nullptr);
  bool callbackExecuted = false;
  web_object_->SetDestroyCallback([&]() { callbackExecuted = true; });
  auto result = web_object_->FireDestroyCallback();
  EXPECT_TRUE(result);
  EXPECT_TRUE(callbackExecuted);
}

TEST_F(ArkWebNativeObjectTest, GetWebInstanceByWebTagTest) {
  web_tag_ = nullptr;
  auto result = web_object_->GetWebInstanceByWebTag(web_tag_);
  EXPECT_EQ(result, nullptr);
}

TEST_F(ArkWebNativeObjectTest, SetWebWeakPtrTest) {
  auto result = web_object_->SetWebWeakPtr(nweb_impl_);
  EXPECT_TRUE(result);
}

TEST_F(ArkWebNativeObjectTest, FireValidCallbackTest_001) {
  bool callbackExecuted = false;
  web_object_->SetValidCallback([&]() { callbackExecuted = true; });
  web_object_->SetWebWeakPtr(nweb_impl_);
  auto result = web_object_->FireValidCallback();
  EXPECT_TRUE(callbackExecuted);
  EXPECT_TRUE(result);
}

TEST_F(ArkWebNativeObjectTest, FireValidCallbackTest_002) {
  auto nweb_impl_ = web_object_->GetWebSharedPtr();
  EXPECT_EQ(nweb_impl_, nullptr);
  auto result = web_object_->FireValidCallback();
  EXPECT_FALSE(result);
}

TEST_F(ArkWebNativeObjectTest, FireLoadStartCallbackTest_001) {
  auto result = web_object_->FireLoadStartCallback();
  EXPECT_FALSE(result);
}

TEST_F(ArkWebNativeObjectTest, FireLoadStartCallbackTest_002) {
  bool callbackExecuted = false;
  web_object_->SetLoadStartCallback([&]() { callbackExecuted = true; });
  auto result = web_object_->FireLoadStartCallback();
  EXPECT_TRUE(result);
  EXPECT_TRUE(callbackExecuted);
}

TEST_F(ArkWebNativeObjectTest, FireLoadEndCallbackTest_001) {
  auto result = web_object_->FireLoadEndCallback();
  EXPECT_FALSE(result);
}

TEST_F(ArkWebNativeObjectTest, FireLoadEndCallbackTest_002) {
  bool callbackExecuted = false;
  web_object_->SetLoadEndCallback([&]() { callbackExecuted = true; });
  auto result = web_object_->FireLoadEndCallback();
  EXPECT_TRUE(result);
  EXPECT_TRUE(callbackExecuted);
}

TEST_F(ArkWebNativeObjectTest, FireDestroyCallbackTest_001) {
  auto result = web_object_->FireDestroyCallback();
  EXPECT_FALSE(result);
}

TEST_F(ArkWebNativeObjectTest, FireDestroyCallbackTest_002) {
  bool callbackExecuted = false;
  web_object_->SetDestroyCallback([&]() { callbackExecuted = true; });
  auto result = web_object_->FireDestroyCallback();
  EXPECT_TRUE(result);
  EXPECT_TRUE(callbackExecuted);
}

}  // namespace OHOS::NWeb