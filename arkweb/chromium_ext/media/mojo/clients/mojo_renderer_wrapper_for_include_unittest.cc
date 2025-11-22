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
#include <cstdlib>
#include <memory>

#include "base/run_loop.h"
#include "base/test/gmock_callback_support.h"
#include "base/test/test_message_loop.h"
#include "media/base/cdm_config.h"
#include "media/base/cdm_context.h"
#include "media/base/mock_filters.h"
#include "media/base/test_helpers.h"
#include "media/cdm/clear_key_cdm_common.h"
#include "media/cdm/default_cdm_factory.h"
#include "media/mojo/mojom/renderer.mojom.h"
#include "media/mojo/services/mojo_cdm_service.h"
#include "media/mojo/services/mojo_cdm_service_context.h"
#include "media/mojo/services/mojo_renderer_service.h"
#include "media/renderers/video_overlay_factory.h"
#include "mojo/public/cpp/bindings/pending_remote.h"
#include "mojo/public/cpp/bindings/receiver.h"
#include "mojo/public/cpp/bindings/remote.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "arkweb/build/features/features.h"

#if BUILDFLAG(ARKWEB_TEST)
#define private public
#include "media/mojo/clients/mojo_renderer_wrapper.h"
#undef private
#else
#include "media/mojo/clients/mojo_renderer_wrapper.h"
#endif //ARKWEB_TEST

using ::base::test::RunOnceCallback;
using ::testing::_;
using ::testing::DoAll;
using ::testing::Return;
using ::testing::SaveArg;
using ::testing::StrictMock;
namespace media {

class TestMojoRendererWrapper : public MojoRendererWrapper {
public:
  explicit TestMojoRendererWrapper(std::unique_ptr<MojoRenderer> mojo_renderer)
      : MojoRendererWrapper(std::move(mojo_renderer)) {}
  RendererType GetRendererType() override {
      return RendererType::kMojo;
  }
};

class MojoRendererWrapperForIncludeTest : public ::testing::Test {
 public:
  MojoRendererWrapperForIncludeTest() {
    std::unique_ptr<StrictMock<MockRenderer>> mock_renderer(
        new StrictMock<MockRenderer>());
    mock_renderer_ = mock_renderer.get();

    renderer_receiver_ = MojoRendererService::Create(
        &mojo_cdm_service_context_, std::move(mock_renderer),
        remote_renderer_remote.InitWithNewPipeAndPassReceiver());

    mojo_renderer_ = std::make_unique<MojoRenderer>(
        message_loop_.task_runner(),
        std::unique_ptr<VideoOverlayFactory>(nullptr), nullptr,
        std::move(remote_renderer_remote));
            mojo_renderer_wrapper_ = std::make_unique<TestMojoRendererWrapper>(std::move(mojo_renderer_));

    EXPECT_CALL(demuxer_, GetAllStreams()).WillRepeatedly(Return(streams_));

    EXPECT_CALL(*mock_renderer_, GetMediaTime())
        .WillRepeatedly(Return(base::TimeDelta()));
  }

  MojoRendererWrapperForIncludeTest(const MojoRendererWrapperForIncludeTest&) = delete;
  MojoRendererWrapperForIncludeTest& operator=(const MojoRendererWrapperForIncludeTest&) = delete;

  ~MojoRendererWrapperForIncludeTest() override = default;

  void Destroy() {
    mojo_renderer_wrapper_.reset();
    base::RunLoop().RunUntilIdle();
  }

  // Completion callbacks.
  MOCK_METHOD1(OnInitialized, void(PipelineStatus));

  std::unique_ptr<StrictMock<MockDemuxerStream>> CreateStream(
      DemuxerStream::Type type) {
    std::unique_ptr<StrictMock<MockDemuxerStream>> stream(
        new StrictMock<MockDemuxerStream>(type));
    return stream;
  }

  void CreateAudioStream() {
    audio_stream_ = CreateStream(DemuxerStream::AUDIO);
    audio_stream_->set_audio_decoder_config(TestAudioConfig::Normal());
    streams_.push_back(audio_stream_.get());
    EXPECT_CALL(demuxer_, GetAllStreams()).WillRepeatedly(Return(streams_));
  }

  void InitializeAndExpect(PipelineStatus status) {
    DVLOG(1) << __func__ << ": " << status;
    EXPECT_CALL(*this, OnInitialized(SameStatusCode(status)));
    mojo_renderer_wrapper_->mojo_renderer_->Initialize(&demuxer_, &renderer_client_,
#if BUILDFLAG(ARKWEB_VIDEO_ASSISTANT)
                               RequestSurfaceCB(),
                               VideoDecoderChangedCB(),
#endif // ARKWEB_VIDEO_ASSISTANT
                               base::BindOnce(&MojoRendererWrapperForIncludeTest::OnInitialized,
                                              base::Unretained(this)));
    base::RunLoop().RunUntilIdle();
  }

  void Initialize() {
    CreateAudioStream();
    EXPECT_CALL(*mock_renderer_, SetVolume(1));
    EXPECT_CALL(*mock_renderer_, OnInitialize(_, _, _))
        .WillOnce(DoAll(SaveArg<1>(&remote_renderer_client_),
                        RunOnceCallback<2>(PIPELINE_OK)));
    InitializeAndExpect(PIPELINE_OK);
  }

  // Fixture members.
  base::TestMessageLoop message_loop_;
  mojo::PendingRemote<mojom::Renderer> remote_renderer_remote;

  // The MojoRenderer that we are testing.
  std::unique_ptr<MojoRenderer> mojo_renderer_;
  std::unique_ptr<TestMojoRendererWrapper> mojo_renderer_wrapper_;

  // Client side mocks and helpers.
  StrictMock<MockRendererClient> renderer_client_;

  // Client side mock demuxer and demuxer streams.
  StrictMock<MockDemuxer> demuxer_;
  std::unique_ptr<StrictMock<MockDemuxerStream>> audio_stream_;
  std::vector<DemuxerStream*> streams_;

  MojoCdmServiceContext mojo_cdm_service_context_;
  std::unique_ptr<MojoCdmService> mojo_cdm_service_;

  raw_ptr<StrictMock<MockRenderer>, AcrossTasksDanglingUntriaged>
      mock_renderer_;
  raw_ptr<RendererClient, DanglingUntriaged> remote_renderer_client_;
  mojo::SelfOwnedReceiverRef<mojom::Renderer> renderer_receiver_;
};

#if BUILDFLAG(ARKWEB_MEDIA_DMABUF)
TEST_F(MojoRendererWrapperForIncludeTest, RecycleDmaBuffer) {
  Initialize();
  EXPECT_CALL(*mock_renderer_, RecycleDmaBuffer()).Times(1);
  EXPECT_NO_FATAL_FAILURE(mojo_renderer_wrapper_->RecycleDmaBuffer());
  Destroy();
}

TEST_F(MojoRendererWrapperForIncludeTest, ResumeDmaBuffer) {
  Initialize();
  EXPECT_CALL(*mock_renderer_, ResumeDmaBuffer()).Times(1);
  EXPECT_NO_FATAL_FAILURE(mojo_renderer_wrapper_->ResumeDmaBuffer());
  Destroy();
}
#endif

}  // namespace media