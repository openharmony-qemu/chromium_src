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

#include "gtest/gtest.h"
#include "base/test/task_environment.h"
#include "gpu/ipc/service/gpu_channel_manager.h"
#include "testing/gmock/include/gmock/gmock.h"

#include <cstdint>
#include "base/run_loop.h"
#include "base/test/test_simple_task_runner.h"
#include "build/build_config.h"
#include "gpu/ipc/common/command_buffer_id.h"
#include "gpu/ipc/common/gpu_channel.mojom.h"
#include "gpu/ipc/service/gpu_channel_manager.h"
#include "gpu/ipc/service/gpu_channel_test_common.h"
#include "gpu/ipc/service/stream_texture_ohos.h"
#include "base/threading/thread.h"
#include "base/logging.h"

#define private public
#include "arkweb/chromium_ext/gpu/ipc/service/gpu_channel_ext.h"
#include "gpu/ipc/service/gpu_channel.h"
#undef private

#define invalidNativeId (-10000)
#define validNativeId 1

namespace gpu {

class GpuChannelOHOSTest : public GpuChannelTestCommon {
 public:
  GpuChannelOHOSTest() : GpuChannelTestCommon(true /* use_stub_bindings */) {}
  ~GpuChannelOHOSTest() override = default;

  void SetUp() {
    channel_ = CreateChannel(kDefaultClientId, true /* is_gpu_host */);
    ASSERT_TRUE(channel_);
  }

  void TearDown() {
    channel_ = nullptr;
  }

  static constexpr int32_t kDefaultClientId = 1;
  GpuChannel* channel_ = nullptr;
};

class GpuChannelExtOHOSTest : public GpuChannelOHOSTest {
protected:
    void SetUp() {
        GpuChannelOHOSTest::SetUp();
        context_state_ = channel_->gpu_channel_manager()->GetSharedContextState(&result_);
        ASSERT_EQ(result_, ContextResult::kSuccess);
        gpu_channel_ext_ = static_cast<GpuChannelExt*>(channel_);
    }

    void TearDown() {
        gpu_channel_ext_ = nullptr;
        GpuChannelOHOSTest::TearDown();
    }
    
    ContextResult result_;
    scoped_refptr<SharedContextState> context_state_;
    GpuChannelExt* gpu_channel_ext_;
};

TEST_F(GpuChannelExtOHOSTest, test001) {
    mojo::PendingAssociatedReceiver<mojom::StreamTexture> receiver;
    int32_t result = gpu_channel_ext_->CreateNativeTexture(
        validNativeId, gl::ohos::TextureOwnerMode::kNativeImageTexture, std::move(receiver));
    EXPECT_NE(result, -1);

    result = gpu_channel_ext_->CreateNativeTexture(
        validNativeId, gl::ohos::TextureOwnerMode::kNativeImageTexture, std::move(receiver));
    EXPECT_EQ(result, -1);
}

TEST_F(GpuChannelExtOHOSTest, test002) {
    auto ret = gpu_channel_ext_->AsGpuChannelExt();
    EXPECT_NE(ret, nullptr);
}

TEST_F(GpuChannelExtOHOSTest, test003) {
    int32_t kClientId = 1;
    bool is_gpu_host = false;
    GpuChannel* channel = CreateChannel(kClientId, is_gpu_host);
    mojo::PendingAssociatedReceiver<mojom::StreamTexture> receiver;
    scoped_refptr<StreamTexture> StreamTexture = StreamTexture::Create(
        channel, 1, gl::ohos::TextureOwnerMode::kNativeImageTexture,
        std::move(receiver));
    EXPECT_NE(StreamTexture, nullptr);
    gpu_channel_ext_->native_textures_[validNativeId] = StreamTexture;
    gpu_channel_ext_->DestroyNativeTexture(invalidNativeId);
    EXPECT_FALSE(gpu_channel_ext_->native_textures_.empty());
}

TEST_F(GpuChannelExtOHOSTest, test004) {
    int32_t kClientId = 1;
    bool is_gpu_host = false;
    GpuChannel* channel = CreateChannel(kClientId, is_gpu_host);
    mojo::PendingAssociatedReceiver<mojom::StreamTexture> receiver;
    scoped_refptr<StreamTexture> StreamTexture = StreamTexture::Create(
        channel, 1, gl::ohos::TextureOwnerMode::kNativeImageTexture,
        std::move(receiver));
    EXPECT_NE(StreamTexture, nullptr);
    gpu_channel_ext_->native_textures_[validNativeId] = StreamTexture;
    gpu_channel_ext_->DestroyNativeTexture(validNativeId);
    EXPECT_TRUE(gpu_channel_ext_->native_textures_.empty());
}

TEST_F(GpuChannelExtOHOSTest, test005_1) {
    gpu_channel_ext_->blankless_dump_info_map_.clear();
    base::ohos::BlanklessDumpInfo info;
    gpu_channel_ext_->SetBlanklessDumpInfo(1, info);
}

TEST_F(GpuChannelExtOHOSTest, test005_2) {
    std::map<uint64_t, base::ohos::BlanklessDumpInfo> map;
    gpu_channel_ext_->blankless_dump_info_map_ .emplace(gpu_channel_ext_->client_id_, map);

    base::ohos::BlanklessDumpInfo info;
    info.dump_enabled = true;
    gpu_channel_ext_->SetBlanklessDumpInfo(1, info);
}

TEST_F(GpuChannelExtOHOSTest, test006_1) {
    base::ohos::BlanklessDumpInfo valid_info;
    base::ohos::BlanklessDumpInfo info;
    std::unordered_map<uint64_t, base::ohos::BlanklessDumpInfo> blankless_dump_info_map_;
    const uint64_t kValidFrameSinkId = 12345;
    uint32_t client_id = 123;
    valid_info.dump_enabled = true;
    blankless_dump_info_map_[kValidFrameSinkId] = valid_info;
    bool result = gpu_channel_ext_->GetBlanklessDumpInfoAndDisableDump(client_id, kValidFrameSinkId, info);
    EXPECT_FALSE(result);
}

TEST_F(GpuChannelExtOHOSTest, test006_2) {
    uint32_t client_id = 2;
    uint64_t frame_sink_id = 2;
    base::ohos::BlanklessDumpInfo info;

    std::map<uint64_t, base::ohos::BlanklessDumpInfo> map;
    gpu_channel_ext_->blankless_dump_info_map_.emplace(0, map);
    bool result = GpuChannelExt::GetBlanklessDumpInfoAndDisableDump(client_id, frame_sink_id, info);
    EXPECT_FALSE(result);
    result = GpuChannelExt::GetBlanklessDumpInfoAndDisableDump(0, frame_sink_id, info);
    EXPECT_FALSE(result);

    gpu_channel_ext_->blankless_dump_info_map_.clear();
    map.emplace(0, info);
    gpu_channel_ext_->blankless_dump_info_map_.emplace(0, map);
    result = GpuChannelExt::GetBlanklessDumpInfoAndDisableDump(0, 0, info);
    EXPECT_FALSE(result);

    gpu_channel_ext_->blankless_dump_info_map_.clear();
    info.dump_enabled = true;
    map.emplace(0, info);
    gpu_channel_ext_->blankless_dump_info_map_.emplace(0, map);
    result = GpuChannelExt::GetBlanklessDumpInfoAndDisableDump(0, 0, info);
    EXPECT_FALSE(result);
}

TEST_F(GpuChannelExtOHOSTest, test006_3) {
    base::ohos::BlanklessDumpInfo info;
    info.dump_enabled = true;
    std::map<uint64_t, base::ohos::BlanklessDumpInfo> map;
    map.emplace(0, info);
    gpu_channel_ext_->blankless_dump_info_map_.clear();
    gpu_channel_ext_->blankless_dump_info_map_.emplace(0, map);
    bool result = GpuChannelExt::GetBlanklessDumpInfoAndDisableDump(0, 0, info);
    EXPECT_TRUE(result);
}
}