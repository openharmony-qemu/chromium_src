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
#include "ui/gl/gl_export.h"
#define private public
#include "ui/gl/ohos/ohos_native_image.h"
#undef private
#include <GL/gl.h>
#include "graphic_adapter.h"

namespace OHOS::NWeb {
namespace gl {
namespace {

class MockNativeImageAdapter : public OHOS::NWeb::NativeImageAdapter {
 public:
  MOCK_METHOD(void,
              CreateNativeImage,
              (uint32_t textureId, uint32_t textureTarget),
              (override));
  MOCK_METHOD(NWebNativeWindow,
              AquireNativeWindowFromNativeImage,
              (),
              (override));
  MOCK_METHOD(int32_t, AttachContext, (uint32_t textureId), (override));
  MOCK_METHOD(int32_t, DetachContext, (), (override));
  MOCK_METHOD(int32_t, UpdateSurfaceImage, (), (override));
  MOCK_METHOD(int64_t, GetTimestamp, (), (override));
  MOCK_METHOD(int32_t, GetTransformMatrix, (float matrix[16]), (override));
  MOCK_METHOD(int32_t, GetSurfaceId, (uint64_t * surfaceId), (override));
  MOCK_METHOD(int32_t,
              SetOnFrameAvailableListener,
              (std::shared_ptr<FrameAvailableListener> listener),
              (override));
  MOCK_METHOD(int32_t, UnsetOnFrameAvailableListener, (), (override));
  MOCK_METHOD(void, DestroyNativeImage, (), (override));
  MOCK_METHOD(void,
              GetNativeWindowBufferSize,
              (void* windowBuffer,
              uint32_t* width,
              uint32_t* height),
              (override));
};

class MockFrameAvailableListener : public FrameAvailableListener {
 public:
  MOCK_METHOD(void*, GetContext, (), (override));
  MOCK_METHOD(OnFrameAvailableCb, GetOnFrameAvailableCb, (), (override));
};

class OhosNativeImageTest : public ::testing::Test {
 protected:
  void SetUp() override {}
  void TearDown() override {}
};

TEST_F(OhosNativeImageTest, SetFrameAvailableCallback_001) {
  auto native_image_adapter = std::make_unique<MockNativeImageAdapter>();
  auto ohos_native_image = ::gl::OhosNativeImage::Create(1);
  ohos_native_image->native_image_adapter_ = std::move(native_image_adapter);
  ohos_native_image->listener_ = nullptr;
  base::RepeatingClosure callback = base::BindRepeating([] {});
  EXPECT_CALL(*static_cast<MockNativeImageAdapter*>(
                  ohos_native_image->native_image_adapter_.get()),
              SetOnFrameAvailableListener(testing::_))
      .Times(1)
      .WillOnce(testing::Return(0));
  ohos_native_image->SetFrameAvailableCallback(std::move(callback));
  testing::Mock::VerifyAndClearExpectations(
      static_cast<MockNativeImageAdapter*>(
          ohos_native_image->native_image_adapter_.get()));
  EXPECT_TRUE(ohos_native_image->frame_available_cb_);
  ohos_native_image->native_image_adapter_ = nullptr;
  ohos_native_image.reset();
}

TEST_F(OhosNativeImageTest, SetFrameAvailableCallback_002) {
  auto native_image_adapter = std::make_unique<MockNativeImageAdapter>();
  auto ohos_native_image = ::gl::OhosNativeImage::Create(1);
  ohos_native_image->native_image_adapter_ = std::move(native_image_adapter);
  ohos_native_image->listener_ =
      std::make_shared<OHOS::NWeb::FrameAvailableListenerImpl>();
  base::RepeatingClosure callback = base::BindRepeating([] {});
  EXPECT_CALL(*static_cast<MockNativeImageAdapter*>(
                  ohos_native_image->native_image_adapter_.get()),
              SetOnFrameAvailableListener(testing::_))
      .Times(0);
  ohos_native_image->SetFrameAvailableCallback(std::move(callback));
  testing::Mock::VerifyAndClearExpectations(
      static_cast<MockNativeImageAdapter*>(
          ohos_native_image->native_image_adapter_.get()));
  ohos_native_image->native_image_adapter_ = nullptr;
  ohos_native_image.reset();
}

TEST_F(OhosNativeImageTest, SetFrameAvailableCallback_003) {
  auto ohos_native_image = ::gl::OhosNativeImage::Create(1);
  ohos_native_image->native_image_adapter_ = nullptr;
  ohos_native_image->listener_ = nullptr;
  base::RepeatingClosure callback = base::BindRepeating([] {});
  ohos_native_image->SetFrameAvailableCallback(std::move(callback));
  EXPECT_EQ(ohos_native_image->listener_, nullptr);
  ohos_native_image.reset();
}

TEST_F(OhosNativeImageTest, SetFrameAvailableCallback_004) {
  auto ohos_native_image = ::gl::OhosNativeImage::Create(1);
  ohos_native_image->native_image_adapter_ = nullptr;
  ohos_native_image->listener_ =
      std::make_shared<OHOS::NWeb::FrameAvailableListenerImpl>();
  base::RepeatingClosure callback = base::BindRepeating([] {});
  ohos_native_image->SetFrameAvailableCallback(std::move(callback));
  EXPECT_NE(ohos_native_image->listener_, nullptr);
  ohos_native_image.reset();
}

TEST_F(OhosNativeImageTest, UpdateNativeImage_001) {
  auto ohos_native_image = ::gl::OhosNativeImage::Create(1);
  auto native_image_adapter = std::make_unique<MockNativeImageAdapter>();
  ohos_native_image->native_image_adapter_ = std::move(native_image_adapter);
  EXPECT_CALL(*static_cast<MockNativeImageAdapter*>(
                  ohos_native_image->native_image_adapter_.get()),
              UpdateSurfaceImage())
      .Times(1);
  ohos_native_image->UpdateNativeImage();
  EXPECT_NE(ohos_native_image->native_image_adapter_, nullptr);
  ohos_native_image->native_image_adapter_ = nullptr;
  ohos_native_image.reset();
}

TEST_F(OhosNativeImageTest, UpdateNativeImage_002) {
  auto ohos_native_image = ::gl::OhosNativeImage::Create(1);
  ohos_native_image->native_image_adapter_ = nullptr;
  ohos_native_image->UpdateNativeImage();
  EXPECT_EQ(ohos_native_image->native_image_adapter_, nullptr);
  ohos_native_image.reset();
}

}  // namespace
}  // namespace gl
}  // namespace OHOS::NWeb
