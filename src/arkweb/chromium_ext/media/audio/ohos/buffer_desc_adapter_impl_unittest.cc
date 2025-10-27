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

#include "buffer_desc_adapter_impl.h"

#include "gtest/gtest.h"

using namespace OHOS::NWeb;

class BufferDescAdapterImplTest : public testing::Test {
 protected:
  BufferDescAdapterImpl* bufferDescAdapterImpl;

  virtual void SetUp() { bufferDescAdapterImpl = new BufferDescAdapterImpl(); }

  virtual void TearDown() { delete bufferDescAdapterImpl; }
};

// Scenario1: Test GetBuffer method
TEST_F(BufferDescAdapterImplTest, SetGetBuffer001) {
  uint8_t* buffer = new uint8_t[1];
  buffer[0] = 1;
  bufferDescAdapterImpl->SetBuffer(buffer);
  auto bufferRes = bufferDescAdapterImpl->GetBuffer();
  ASSERT_EQ(buffer[0], bufferRes[0]);
  delete[] buffer;
}

// Scenario1: Test SetBufLength method
TEST_F(BufferDescAdapterImplTest, SetGetBufLength001) {
  int32_t length = 5;  // 5 for length test
  bufferDescAdapterImpl->SetBufLength(length);
  ASSERT_EQ(length, bufferDescAdapterImpl->GetBufLength());
}

// Scenario1: Test SetDataLength method
TEST_F(BufferDescAdapterImplTest, SetGetDataLength001) {
  int32_t length = 5;  // 5 for length test
  bufferDescAdapterImpl->SetDataLength(length);
  ASSERT_EQ(length, bufferDescAdapterImpl->GetDataLength());
}
