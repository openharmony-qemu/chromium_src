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
#include "ohos_sdk/openharmony/native/llvm/bin/../include/libcxx-ohos/include/c++/v1/__ranges/lazy_split_view.h"
#define private public
#include "content/renderer/media/ohos/ohos_custom_media_player_renderer_client.h"
#undef private

#include <memory>
#include <utility>

#include "base/functional/bind.h"
#include "base/task/single_thread_task_runner.h"
#include "media/base/mock_filters.h"
#include "media/base/mock_video_renderer_sink.h"
#include "media/mojo/clients/mojo_renderer.h"
#include "media/mojo/mojom/renderer_extensions.mojom.h"
#include "mojo/public/cpp/system/simple_watcher.h"
#include "media/mojo/clients/mojo_renderer_wrapper.h"
#include "testing/gmock/include/gmock/gmock.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "third_party/skia/include/core/SkBitmap.h"
#include "ui/gfx/geometry/rect.h"
#include "ui/gfx/geometry/size.h"
#include "media/base/native_texture_wrapper.h"
#include "content/renderer/media/ohos/native_texture_factory.h"
#include "base/test/task_environment.h"
#include "media/base/video_renderer_sink.h"
#include "media/renderers/video_overlay_factory.h"
#include "media/base/media_resource.h"
#include "arkweb/chromium_ext/media/base/renderer_ext.h"

using ::testing::_;
using ::testing::ByMove;
using ::testing::DoAll;
using ::testing::Invoke;
using ::testing::Return;
using ::testing::ReturnRef;
using ::testing::SaveArg;
using ::testing::StrictMock;

namespace content {

class MockNativeMediaResource : public media::MediaResource {
public:
  MockNativeMediaResource() = default;
  ~MockNativeMediaResource() override = default;

  // MediaResource implementation.
  MOCK_CONST_METHOD0(GetType, media::MediaResource::Type());
  MOCK_METHOD0(GetAllStreams, std::vector<media::DemuxerStream*>());
  MOCK_METHOD1(GetFirstStream, media::DemuxerStream*(media::DemuxerStream::Type type));
  MOCK_CONST_METHOD0(GetMediaUrlParams, const media::MediaUrlParams&());
  MOCK_METHOD1(ForwardDurationChangeToDemuxerHost, void(base::TimeDelta duration));
  MOCK_METHOD1(SetHeaders, void(base::flat_map<std::string, std::string> headers));
  MOCK_METHOD1(ForwardBufferedEndTimeChangeToDemuxerHost, void(base::TimeDelta buffered_time));
  MOCK_METHOD1(SetPreloadType, void(uint32_t preload_type));
  MOCK_METHOD1(SetMediaSourceType, void(uint32_t media_source_type));
};

class MockNativeTextureWrapper : public media::NativeTextureWrapper {
public:
    MockNativeTextureWrapper() = default;
    ~MockNativeTextureWrapper() override = default;

    // Make Initialize more flexible with default actions
    MOCK_METHOD5(Initialize, void(const base::RepeatingClosure& received_frame_cb,
        scoped_refptr<base::SingleThreadTaskRunner> compositor_task_runner,
        StreamTextureWrapperInitCB init_cb,
        CreateSurfaceTextureCB create_surface_cb,
        DestroyTextureCB destroy_surface_cb));

    MOCK_METHOD1(UpdateTextureSize, void(const gfx::Size& size));
    MOCK_METHOD0(GetCurrentFrame, scoped_refptr<media::VideoFrame>());
    MOCK_METHOD0(ClearCBOnAnyThread, void());
    MOCK_METHOD0(Destroy, void());
};

class MockMojoRenderer : public media::MojoRenderer {
public:
    MockMojoRenderer(scoped_refptr<base::SequencedTaskRunner> task_runner, media::VideoRendererSink* sink) :
        media::MojoRenderer(std::move(task_runner), 
        std::make_unique<media::VideoOverlayFactory>(),
        sink, 
        mojo::NullRemote()) {}

    ~MockMojoRenderer() override = default;

    MOCK_METHOD5(Initialize,
                void(media::MediaResource* media_resource,
                media::RendererClient* client,
                media::RequestSurfaceCB request_surface_cb,
                media::VideoDecoderChangedCB decoder_changed_cb,
                media::PipelineStatusCallback init_cb));

    MOCK_METHOD1(Flush, void(base::OnceClosure flush_cb));
    MOCK_METHOD1(StartPlayingFrom, void(base::TimeDelta time));
    MOCK_METHOD1(SetPlaybackRate, void(double playback_rate));
    MOCK_METHOD1(SetVolume, void(float volume));
    MOCK_METHOD1(SetMuted, void(bool muted));
    MOCK_METHOD0(GetMediaTime, base::TimeDelta());
    MOCK_METHOD2(SetSurfaceId, void(int surface_id, const gfx::Rect& rect));
};

class MockRendererClient : public media::RendererClient {
public:
    MockRendererClient() = default;
    virtual ~MockRendererClient() = default;

    MOCK_METHOD1(OnError, void(media::PipelineStatus status));
    MOCK_METHOD1(OnFallback, void(media::PipelineStatus status));
    MOCK_METHOD0(OnEnded, void());
    MOCK_METHOD1(OnStatisticsUpdate, void(const media::PipelineStatistics& stats));
    MOCK_METHOD2(OnBufferingStateChange, void(media::BufferingState state, media::BufferingStateChangeReason reason));
    MOCK_METHOD1(OnWaiting, void(media::WaitingReason reason));
    MOCK_METHOD1(OnAudioConfigChange, void(const media::AudioDecoderConfig& config));
    MOCK_METHOD1(OnVideoConfigChange, void(const media::VideoDecoderConfig& config));
    MOCK_METHOD1(OnVideoNaturalSizeChange, void(const gfx::Size& size));
    MOCK_METHOD1(OnVideoOpacityChange, void(bool opaque));
    MOCK_METHOD0(IsVideoStreamAvailable, bool());
    MOCK_METHOD1(OnVideoFrameRateChange, void(std::optional<int> fps));
};

class OHOSCustomMediaPlayerRendererClientTest
    : public ::testing::Test {
protected:
    OHOSCustomMediaPlayerRendererClientTest() {
        media_task_runner_ = base::SequencedTaskRunner::GetCurrentDefault();
        compositor_task_runner_ = task_environment_.GetMainThreadTaskRunner();
        mock_video_renderer_sink_ = std::make_unique<media::MockVideoRendererSink>();
        mock_mojo_renderer_ = std::make_unique<MockMojoRenderer>(base::SequencedTaskRunner::GetCurrentDefault(),
            mock_video_renderer_sink_.get());
        mock_native_texture_wrapper_ = std::make_unique<MockNativeTextureWrapper>();
    }

    ~OHOSCustomMediaPlayerRendererClientTest() override = default;

    void SetUp() override {

        renderer_client_ = std::make_unique<MockRendererClient>();

        mojo::PendingRemote<media::mojom::MediaPlayerRendererExtension> renderer_extension_remote;
        auto renderer_extension_receiver = renderer_extension_remote.InitWithNewPipeAndPassReceiver();

        mojo::PendingRemote<media::mojom::CustomMediaPlayerRendererClientExtension> client_extension_remote;
        auto client_extension_receiver = client_extension_remote.InitWithNewPipeAndPassReceiver();

        client_ = std::make_unique<OHOSCustomMediaPlayerRendererClient>(
            std::move(renderer_extension_remote),
            std::move(client_extension_receiver),
            media_task_runner_, compositor_task_runner_,
            std::move(mock_mojo_renderer_),
            media::ScopedNativeTextureWrapper(mock_native_texture_wrapper_.get()),
            mock_video_renderer_sink_.get()
        );

        media_resource_ = std::make_unique<MockNativeMediaResource>();
            // Create media resource and set it as a raw pointer (will be cleaned up in TearDown)
        client_->media_resource_ = media_resource_.get();
    }

    void TearDown() override {
        media_resource_.reset();
        client_.reset();
    }

    base::test::SingleThreadTaskEnvironment task_environment_;

    scoped_refptr<base::SequencedTaskRunner> media_task_runner_;
    scoped_refptr<base::SingleThreadTaskRunner> compositor_task_runner_;

    std::unique_ptr<MockRendererClient> renderer_client_;
    std::unique_ptr<OHOSCustomMediaPlayerRendererClient> client_;

    std::unique_ptr<MockMojoRenderer> mock_mojo_renderer_;
    std::unique_ptr<MockNativeTextureWrapper> mock_native_texture_wrapper_;
    std::unique_ptr<media::MockVideoRendererSink> mock_video_renderer_sink_;
    std::unique_ptr<MockNativeMediaResource> media_resource_;
};

// Test constructor
TEST_F(OHOSCustomMediaPlayerRendererClientTest, Constructor) {
    EXPECT_TRUE(client_ != nullptr);
    EXPECT_EQ(client_->GetRendererType(), media::RendererType::kOHOSCustomMediaPlayer);
}

// Test destructor
TEST_F(OHOSCustomMediaPlayerRendererClientTest, Destructor) {
    EXPECT_CALL(*mock_native_texture_wrapper_, Destroy()).Times(1);
    client_.reset();
}

// Test successful initialization
TEST_F(OHOSCustomMediaPlayerRendererClientTest, Initialize_Success) {
    // media::MediaResource media_resource(GURL("http://test.com"), 0, 0);
    media::PipelineStatusCallback init_cb =
        base::BindOnce([](media::PipelineStatus status) {});

    EXPECT_CALL(*mock_native_texture_wrapper_, Initialize(
        testing::_, testing::_, testing::_, testing::_, testing::_))
        .WillOnce(testing::Return());

    media::RequestSurfaceCB request_surface_cb;
    media::VideoDecoderChangedCB decoder_changed_cb;
    client_->Initialize(media_resource_.get(), renderer_client_.get(), 
                        std::move(request_surface_cb), std::move(decoder_changed_cb),
                        base::BindOnce([](media::PipelineStatus status) {}));
    task_environment_.RunUntilIdle();
}

// Test initialization failure
TEST_F(OHOSCustomMediaPlayerRendererClientTest, Initialize_Failure) {
    // media::MediaResource media_resource(GURL("http://test.com"), 0, 0);
    media::PipelineStatusCallback init_cb =
        base::BindOnce([](media::PipelineStatus status) {});

    EXPECT_CALL(*mock_native_texture_wrapper_, Initialize(
        testing::_, testing::_, testing::_, testing::_, testing::_))
        .WillOnce(testing::Return());

    media::PipelineStatusCallback cb =
        base::BindOnce([](media::PipelineStatus status) {
            EXPECT_EQ(status, media::PipelineStatus::Codes::PIPELINE_ERROR_INITIALIZATION_FAILED);
        });
    media::RequestSurfaceCB request_surface_cb;
    media::VideoDecoderChangedCB decoder_changed_cb;
    client_->Initialize(media_resource_.get(), renderer_client_.get(), 
                        std::move(request_surface_cb), std::move(decoder_changed_cb), std::move(cb));

    task_environment_.RunUntilIdle();
}

// Test renderer initialization failure
TEST_F(OHOSCustomMediaPlayerRendererClientTest, Initialize_RendererFailure) {
    EXPECT_CALL(*mock_native_texture_wrapper_, Initialize(
        testing::_, testing::_, testing::_, testing::_, testing::_))
        .WillOnce(testing::Return());

    media::PipelineStatusCallback cb =
        base::BindOnce([](media::PipelineStatus status) {
            EXPECT_EQ(status, media::PipelineStatus::Codes::PIPELINE_ERROR_INITIALIZATION_FAILED_CUSTOM_PLAYER);
        });

    media::RequestSurfaceCB request_surface_cb;
    media::VideoDecoderChangedCB decoder_changed_cb;
    client_->Initialize(media_resource_.get(), renderer_client_.get(), 
        std::move(request_surface_cb), std::move(decoder_changed_cb), 
        std::move(cb));
    task_environment_.RunUntilIdle();
}

// Test OnVideoSizeChange
TEST_F(OHOSCustomMediaPlayerRendererClientTest, OnVideoSizeChange) {
    gfx::Size size(1280, 720);
    EXPECT_CALL(*renderer_client_, OnVideoNaturalSizeChange(size)).Times(1);

    media::RequestSurfaceCB request_surface_cb;
    media::VideoDecoderChangedCB decoder_changed_cb;
    client_->Initialize(media_resource_.get(), renderer_client_.get(), 
                        std::move(request_surface_cb), std::move(decoder_changed_cb),
                        base::BindOnce([](media::PipelineStatus status) {}));
    client_->OnVideoSizeChange(size);
    task_environment_.RunUntilIdle();
}

// Test OnDurationChange
TEST_F(OHOSCustomMediaPlayerRendererClientTest, OnDurationChange_WithMediaResource) {
    base::TimeDelta duration = base::Seconds(60);
    EXPECT_CALL(*(media_resource_.get()), ForwardDurationChangeToDemuxerHost(duration))
        .Times(1);

    media::RequestSurfaceCB request_surface_cb;
    media::VideoDecoderChangedCB decoder_changed_cb;
    client_->Initialize(media_resource_.get(), renderer_client_.get(), 
                        std::move(request_surface_cb), std::move(decoder_changed_cb),
                        base::BindOnce([](media::PipelineStatus status) {}));
    client_->OnDurationChange(duration);
    task_environment_.RunUntilIdle();
}

// Test UpdatePlaybackStatus
TEST_F(OHOSCustomMediaPlayerRendererClientTest, UpdatePlaybackStatus_WithCallback) {
    media::Renderer::UpdatePlaybackStatusCallback cb = base::BindRepeating([](uint32_t value) {
      uint32_t expect_value = 1;
      EXPECT_EQ(value, expect_value);
    });

    media::RequestSurfaceCB request_surface_cb;
    media::VideoDecoderChangedCB decoder_changed_cb;
    client_->Initialize(media_resource_.get(), renderer_client_.get(), 
                        std::move(request_surface_cb), std::move(decoder_changed_cb),
                        base::BindOnce([](media::PipelineStatus status) {}));

    client_->SetUpdatePlaybackStatusCallback(std::move(cb));
    client_->UpdatePlaybackStatus(1);
    task_environment_.RunUntilIdle();
}

// Test UpdateVolume
TEST_F(OHOSCustomMediaPlayerRendererClientTest, UpdateVolume_WithCallback) {
    media::Renderer::UpdateVolumeCallback cb = base::BindRepeating([](double volume) {
        double expect_value = 0.5;
        EXPECT_DOUBLE_EQ(volume, expect_value); 
    });

    media::RequestSurfaceCB request_surface_cb;
    media::VideoDecoderChangedCB decoder_changed_cb;
    client_->Initialize(media_resource_.get(), renderer_client_.get(), 
                        std::move(request_surface_cb), std::move(decoder_changed_cb),
                        base::BindOnce([](media::PipelineStatus status) {}));
    client_->SetUpdateVolumeCallback(std::move(cb));
    client_->UpdateVolume(0.5);
    task_environment_.RunUntilIdle();
}

// Test UpdateMuted
TEST_F(OHOSCustomMediaPlayerRendererClientTest, UpdateMuted_WithCallback) {
    media::Renderer::UpdateMutedCallback cb = base::BindRepeating([](bool muted) {
        bool expect_value = true;
        EXPECT_EQ(muted, expect_value);
    });

    media::RequestSurfaceCB request_surface_cb;
    media::VideoDecoderChangedCB decoder_changed_cb;
    client_->Initialize(media_resource_.get(), renderer_client_.get(), 
                        std::move(request_surface_cb), std::move(decoder_changed_cb),
                        base::BindOnce([](media::PipelineStatus status) {}));
    client_->SetUpdateMutedCallback(std::move(cb));
    client_->UpdateMuted(true);
    task_environment_.RunUntilIdle();
}


// Test UpdatePlaybackRate
TEST_F(OHOSCustomMediaPlayerRendererClientTest, UpdatePlaybackRate_WithCallback) {
    media::Renderer::UpdatePlaybackRateCallback cb = base::BindRepeating([](double playback_rate) {
        double expect_value = 1.5;
        EXPECT_DOUBLE_EQ(playback_rate, expect_value);
    });

    media::RequestSurfaceCB request_surface_cb;
    media::VideoDecoderChangedCB decoder_changed_cb;
    client_->Initialize(media_resource_.get(), renderer_client_.get(), 
                        std::move(request_surface_cb), std::move(decoder_changed_cb),
                        base::BindOnce([](media::PipelineStatus status) {}));
    client_->SetUpdatePlaybackRateCallback(std::move(cb));
    client_->UpdatePlaybackRate(1.5);
    task_environment_.RunUntilIdle();
}

// Test UpdateBufferedEndTime_WithMediaResource
TEST_F(OHOSCustomMediaPlayerRendererClientTest, UpdateBufferedEndTime_WithMediaResource) {
    base::TimeDelta buffered_time = base::Seconds(30);
    EXPECT_CALL(*(media_resource_.get()), ForwardBufferedEndTimeChangeToDemuxerHost(buffered_time))
        .Times(1);

    media::RequestSurfaceCB request_surface_cb;
    media::VideoDecoderChangedCB decoder_changed_cb;
    client_->Initialize(media_resource_.get(), renderer_client_.get(), 
                        std::move(request_surface_cb), std::move(decoder_changed_cb),
                        base::BindOnce([](media::PipelineStatus status) {}));
    client_->UpdateBufferedEndTime(30.0);
    task_environment_.RunUntilIdle();
}

// Test OnSurfaceCreated_WithCallback
TEST_F(OHOSCustomMediaPlayerRendererClientTest, OnSurfaceCreated_WithCallback) {
    media::Renderer::SurfaceCreatedCallback cb = base::BindRepeating(
        [](int surface_id, media::Renderer::OnGetRectCallback get_rect_cb) {
            EXPECT_EQ(surface_id, 1);
    });

    media::RequestSurfaceCB request_surface_cb;
    media::VideoDecoderChangedCB decoder_changed_cb;
    client_->Initialize(media_resource_.get(), renderer_client_.get(), 
                        std::move(request_surface_cb), std::move(decoder_changed_cb),
                        base::BindOnce([](media::PipelineStatus status) {}));
    client_->SetSurfaceCreatedCallback(std::move(cb));
    client_->surface_id_ = 1;

    client_->OnSurfaceCreated(1);
    task_environment_.RunUntilIdle();
}

// Test OnSurfaceCreated_WithoutCallback
TEST_F(OHOSCustomMediaPlayerRendererClientTest, OnSurfaceCreated_WithoutCallback) {
    media::RequestSurfaceCB request_surface_cb;
    media::VideoDecoderChangedCB decoder_changed_cb;
    client_->Initialize(media_resource_.get(), renderer_client_.get(), 
                        std::move(request_surface_cb), std::move(decoder_changed_cb),
                        base::BindOnce([](media::PipelineStatus status) {}));
    client_->surface_id_ = 1;
    EXPECT_NO_FATAL_FAILURE(client_->OnSurfaceCreated(1));
    task_environment_.RunUntilIdle();
}

// Test OnSurfaceDestroyed
TEST_F(OHOSCustomMediaPlayerRendererClientTest, OnSurfaceDestroyed) {
    EXPECT_NO_FATAL_FAILURE(client_->OnSurfaceDestroyed());
}

// Test OnFrameAvailable
TEST_F(OHOSCustomMediaPlayerRendererClientTest, OnFrameAvailable) {
    auto mock_frame = media::VideoFrame::CreateFrame(
        media::PIXEL_FORMAT_I420,  // media::VideoPixelFormat
        gfx::Size(640, 480),       // coded_size
        gfx::Rect(640, 480),       // visible_rect
        gfx::Size(640, 480),       // natural_size
        base::TimeDelta()          // timestamp
    );

    EXPECT_CALL(*mock_native_texture_wrapper_, GetCurrentFrame())
        .WillOnce(Return(mock_frame));

    EXPECT_CALL(*mock_video_renderer_sink_, PaintSingleFrame(_, _))
        .WillOnce(Invoke([mock_frame](scoped_refptr<media::VideoFrame> frame, bool repainting) {
            EXPECT_NE(frame.get(), mock_frame.get());
            EXPECT_EQ(frame->natural_size(), mock_frame->natural_size());
        }));

    media::RequestSurfaceCB request_surface_cb;
    media::VideoDecoderChangedCB decoder_changed_cb;
    client_->Initialize(media_resource_.get(), renderer_client_.get(), 
                        std::move(request_surface_cb), std::move(decoder_changed_cb),
                        base::BindOnce([](media::PipelineStatus status) {}));
    client_->OnFrameAvailable();
    task_environment_.RunUntilIdle();
}

// Test OnGetVideoRect_FirstCall
TEST_F(OHOSCustomMediaPlayerRendererClientTest, OnGetVideoRect_FirstCall) {
    gfx::Rect rect(100, 100);

    EXPECT_CALL(*mock_native_texture_wrapper_, UpdateTextureSize(rect.size())).Times(1);

    media::RequestSurfaceCB request_surface_cb;
    media::VideoDecoderChangedCB decoder_changed_cb;
    client_->Initialize(media_resource_.get(), renderer_client_.get(), 
                        std::move(request_surface_cb), std::move(decoder_changed_cb),
                        base::BindOnce([](media::PipelineStatus status) {}));
    client_->surface_id_ = 1;
    client_->has_sent_surface_id_to_remote_ = true;
    client_->OnGetVideoRect(rect);
}

// Test OnGetVideoRect_SecondCall
TEST_F(OHOSCustomMediaPlayerRendererClientTest, OnGetVideoRect_SecondCall) {
    gfx::Rect rect(200, 200);

    EXPECT_CALL(*mock_native_texture_wrapper_, UpdateTextureSize(rect.size())).Times(1);

    media::RequestSurfaceCB request_surface_cb;
    media::VideoDecoderChangedCB decoder_changed_cb;
    client_->Initialize(media_resource_.get(), renderer_client_.get(), 
                        std::move(request_surface_cb), std::move(decoder_changed_cb),
                        base::BindOnce([](media::PipelineStatus status) {}));
    client_->surface_id_ = 1;
    client_->has_sent_surface_id_to_remote_ = false;
    client_->OnGetVideoRect(rect);
}

// Test SetUpdatePlaybackStatusCallback
TEST_F(OHOSCustomMediaPlayerRendererClientTest, SetUpdatePlaybackStatusCallback) {
    media::Renderer::UpdatePlaybackStatusCallback cb = base::BindRepeating([](uint32_t value) {
      uint32_t expect_value = 1;
      EXPECT_EQ(value, expect_value);
    });

    media::RequestSurfaceCB request_surface_cb;
    media::VideoDecoderChangedCB decoder_changed_cb;
    client_->Initialize(media_resource_.get(), renderer_client_.get(), 
                        std::move(request_surface_cb), std::move(decoder_changed_cb),
                        base::BindOnce([](media::PipelineStatus status) {}));
    client_->SetUpdatePlaybackStatusCallback(std::move(cb));
    bool is_callback_empty = client_->update_playback_status_cb_.is_null();
    EXPECT_FALSE(is_callback_empty);
}

// Test SetUpdateVolumeCallback
TEST_F(OHOSCustomMediaPlayerRendererClientTest, SetUpdateVolumeCallback) {
    media::Renderer::UpdateVolumeCallback cb = base::BindRepeating([](double volume) {
        double expect_value = 0.5;
        EXPECT_DOUBLE_EQ(volume, expect_value); 
    });

    media::RequestSurfaceCB request_surface_cb;
    media::VideoDecoderChangedCB decoder_changed_cb;
    client_->Initialize(media_resource_.get(), renderer_client_.get(), 
                        std::move(request_surface_cb), std::move(decoder_changed_cb),
                        base::BindOnce([](media::PipelineStatus status) {}));
    client_->SetUpdateVolumeCallback(std::move(cb));
    bool is_callback_empty = client_->update_volume_cb_.is_null();
    EXPECT_FALSE(is_callback_empty);
}

// Test SetUpdateMutedCallback
TEST_F(OHOSCustomMediaPlayerRendererClientTest, SetUpdateMutedCallback) {
    media::Renderer::UpdateMutedCallback cb = base::BindRepeating([](bool muted) {
        bool expect_value = true;
        EXPECT_EQ(muted, expect_value);
    });

    media::RequestSurfaceCB request_surface_cb;
    media::VideoDecoderChangedCB decoder_changed_cb;
    client_->Initialize(media_resource_.get(), renderer_client_.get(), 
                        std::move(request_surface_cb), std::move(decoder_changed_cb),
                        base::BindOnce([](media::PipelineStatus status) {}));
    client_->SetUpdateMutedCallback(std::move(cb));
    bool is_callback_empty = client_->update_muted_cb_.is_null();
    EXPECT_FALSE(is_callback_empty);
}

// Test SetUpdatePlaybackRateCallback
TEST_F(OHOSCustomMediaPlayerRendererClientTest, SetUpdatePlaybackRateCallback) {
    media::Renderer::UpdatePlaybackRateCallback cb = base::BindRepeating([](double playback_rate) {
        double expect_value = 1.5;
        EXPECT_DOUBLE_EQ(playback_rate, expect_value);
    });

    media::RequestSurfaceCB request_surface_cb;
    media::VideoDecoderChangedCB decoder_changed_cb;
    client_->Initialize(media_resource_.get(), renderer_client_.get(), 
                        std::move(request_surface_cb), std::move(decoder_changed_cb),
                        base::BindOnce([](media::PipelineStatus status) {}));
    client_->SetUpdatePlaybackRateCallback(std::move(cb));
    bool is_callback_empty = client_->update_playback_rate_cb_.is_null();
    EXPECT_FALSE(is_callback_empty);
}

// Test renderer extension binding functionality
TEST_F(OHOSCustomMediaPlayerRendererClientTest, RendererExtension_Binding) {
    // Test SetSurfaceId with rect
    gfx::Rect test_rect(100, 200, 300, 400);

    media::RequestSurfaceCB request_surface_cb;
    media::VideoDecoderChangedCB decoder_changed_cb;
    client_->Initialize(media_resource_.get(), renderer_client_.get(), 
                        std::move(request_surface_cb), std::move(decoder_changed_cb),
                        base::BindOnce([](media::PipelineStatus status) {}));
    client_->surface_id_ = 1;
    client_->has_sent_surface_id_to_remote_ = false;
    client_->OnGetVideoRect(test_rect);
    EXPECT_TRUE(client_->has_sent_surface_id_to_remote_);
    // Verify that subsequent calls don't re-send surface id
    client_->OnGetVideoRect(test_rect);
    EXPECT_TRUE(client_->has_sent_surface_id_to_remote_);
}

}  // namespace content