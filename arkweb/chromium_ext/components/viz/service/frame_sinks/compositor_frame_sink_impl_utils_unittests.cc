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

#include "arkweb/chromium_ext/components/viz/service/frame_sinks/compositor_frame_sink_impl_utils.h"
#include "testing/gmock/include/gmock/gmock.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "components/viz/test/mock_compositor_frame_sink_client.h"
#include "components/viz/service/frame_sinks/frame_sink_manager_impl.h"
#include "components/viz/common/frame_sinks/begin_frame_source.h"
#include "ash/frame_sink/test/test_begin_frame_source.h"

namespace viz {

class CompositorFrameSinkImplUtilTest : public testing::Test {
public:
    CompositorFrameSinkImplUtilTest() = default;
    ~CompositorFrameSinkImplUtilTest() = default;
};

#if BUILDFLAG(ARKWEB_REPORT_SYS_EVENT)
TEST_F(CompositorFrameSinkImplUtilTest, SubmitCompositorFrameUtils) {
    CompositorFrameMetadata metadata;
    CompositorFrameSinkImplUtil util(nullptr);

    metadata.dropped_frame_count = 0;
    metadata.dropped_frame_duration = 0;
    ASSERT_NO_FATAL_FAILURE(util.SubmitCompositorFrameUtils(metadata));

    metadata.dropped_frame_count = 1;
    metadata.dropped_frame_duration = 0;
    ASSERT_NO_FATAL_FAILURE(util.SubmitCompositorFrameUtils(metadata));

    metadata.dropped_frame_count = 0;
    metadata.dropped_frame_duration = 1;
    ASSERT_NO_FATAL_FAILURE(util.SubmitCompositorFrameUtils(metadata));

    metadata.dropped_frame_count = 1;
    metadata.dropped_frame_duration = 1;
    ASSERT_NO_FATAL_FAILURE(util.SubmitCompositorFrameUtils(metadata));
}
#endif

#if BUILDFLAG(ARKWEB_REPORT_SYS_EVENT)
TEST_F(CompositorFrameSinkImplUtilTest, ReportKeyThreadIdsUtils) {
    CompositorFrameSinkImplUtil util(nullptr);
    ASSERT_NO_FATAL_FAILURE(util.ReportKeyThreadIdsUtils({}, 0, false));
    #if BUILDFLAG(ARKWEB_OOP_GPU_PROCESS)
    ASSERT_NO_FATAL_FAILURE(util.ReportKeyThreadIdsUtils({0}, 0, true));
    #endif
}
#endif

#if BUILDFLAG(ARKWEB_VIDEO_LTPO)
TEST_F(CompositorFrameSinkImplUtilTest, GetFrameRate){
    CompositorFrameSinkImplUtil util1(nullptr);
    EXPECT_EQ(util1.GetFrameRate(), 0);

    FrameSinkId frame_sink_id;
    MockCompositorFrameSinkClient compositor_frame_sink_client;
    mojo::Remote<mojom::CompositorFrameSink> compositor_frame_sink;

    auto frame_sink_manager_impl = std::make_unique<viz::FrameSinkManagerImpl>(viz::FrameSinkManagerImpl::InitParams());
    auto compositor_frame_sink_impl = std::make_unique<CompositorFrameSinkImpl>(
        frame_sink_manager_impl.get(), frame_sink_id, std::nullopt,
        compositor_frame_sink.BindNewPipeAndPassReceiver(),
        compositor_frame_sink_client.BindInterfaceRemote());

    EXPECT_NE(compositor_frame_sink_impl, nullptr);
    CompositorFrameSinkImplUtil util2(compositor_frame_sink_impl.get());
    EXPECT_EQ(util2.GetFrameRate(), 0);
}
#endif

#if BUILDFLAG(ARKWEB_VSYNC_SCHEDULE)
TEST_F(CompositorFrameSinkImplUtilTest, OnSetBypassVsyncCondition) {
    CompositorFrameSinkImplUtil util(nullptr);
    ASSERT_NO_FATAL_FAILURE(util.OnSetBypassVsyncCondition(0));
}
#endif

#if BUILDFLAG(ARKWEB_THROTTLE_FRAME)
TEST_F(CompositorFrameSinkImplUtilTest, UpdateThrottleMode) {
    CompositorFrameSinkImplUtil util(nullptr);
    ASSERT_NO_FATAL_FAILURE(util.UpdateThrottleMode(false));
}
#endif
} //namespace viz