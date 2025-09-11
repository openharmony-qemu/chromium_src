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

#include "arkweb/ohos_adapter_ndk/interfaces/ohos_adapter_helper.h"
#include "arkweb/ohos_adapter_ndk/interfaces/mock/mock_ohos_adapter_helper.h"
#define private public
#include "arkweb/chromium_ext/base/ohos/scoped_native_buffer_handle.h"
#undef private
#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "base/logging.h"
using namespace gpu;
using namespace OHOS::NWeb;
namespace OHOS::NWeb {
class MockOhosNativeBufferAdapter : public OhosNativeBufferAdapter {
public:
  MOCK_METHOD(void, AcquireBuffer, (void* buffer), (override));
  MOCK_METHOD(void, Release, (void* buffer), (override));
  MOCK_METHOD(int, GetEGLBuffer, (void* buffer, void** eglBuffer), (override));
  MOCK_METHOD(int, FreeEGLBuffer, (void* eglBuffer), (override));
  MOCK_METHOD(int, NativeBufferFromNativeWindowBuffer, (void* nativeWindowBuffer, void** nativeBuffer), (override));
  MOCK_METHOD(uint32_t, GetSeqNum, (void* nativeBuffer), (override));
  MOCK_METHOD(void, Allocate, (const std::shared_ptr<NativeBufferConfigAdapter> bufferConfig, void** outBuffer), (override));
  MOCK_METHOD(void, Describe, (std::shared_ptr<NativeBufferConfigAdapter> bufferConfig, void* buffer), (override));
  MOCK_METHOD(int, Lock, (void* buffer, uint64_t usage, int32_t fence, void** out_virtual_address), (override));
  MOCK_METHOD(int, RecvHandleFromUnixSocket, (int socketFd, void** outBuffer), (override));
  MOCK_METHOD(int, SendHandleToUnixSocket, (const void* buffer, int socketFd), (override));
  MOCK_METHOD(int, Unlock, (void* buffer, int32_t* fence), (override));
  MOCK_METHOD(int, FreeNativeBuffer, (void* nativeBuffer), (override));
};
}

namespace base {
namespace ohos {
class ScopedNativeBufferHandleTest : public ::testing::Test {
protected:
  void SetUp() override {
    native_buffer_ = malloc(1024);
  }
  void TearDown() override {
    free(native_buffer_);
  }

  OHOSNativeBuffer native_buffer_ = nullptr;
  ScopedNativeBufferHandle handle_;
};

TEST_F(ScopedNativeBufferHandleTest, reset001) {
  MockOhosAdapterHelper* instance = new MockOhosAdapterHelper();
  OhosAdapterHelper::SetInstance(instance);
  MockOhosNativeBufferAdapter adapter;
  ScopedNativeBufferHandle handle;
  handle.reset();
  EXPECT_EQ(handle.buffer_, nullptr);
  OhosAdapterHelper::SetInstance(nullptr);
  delete instance;
}

TEST_F(ScopedNativeBufferHandleTest, reset002) {
  MockOhosAdapterHelper* instance = new MockOhosAdapterHelper();
  OhosAdapterHelper::SetInstance(instance);
  MockOhosNativeBufferAdapter adapter;
  EXPECT_CALL(*instance, GetOhosNativeBufferAdapter())
    .WillOnce(testing::ReturnRef(adapter));
  EXPECT_CALL(adapter, Release(native_buffer_)).Times(1);
  
  ScopedNativeBufferHandle handle_;
  handle_.buffer_ = native_buffer_;
  handle_.reset();
  EXPECT_EQ(handle_.buffer_, nullptr);
  OhosAdapterHelper::SetInstance(nullptr);
  delete instance;
}

TEST_F(ScopedNativeBufferHandleTest, Take) {
  handle_.buffer_ = native_buffer_;
  OHOSNativeBuffer buffer = nullptr;
  buffer = handle_.Take();
  EXPECT_EQ(buffer, native_buffer_);
}

}  // namespace ohos
}  // namespace base
