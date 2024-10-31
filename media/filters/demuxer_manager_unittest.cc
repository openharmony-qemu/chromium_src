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

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#define private public
#include "media/base/data_source.h"
#include "media/filters/demuxer_manager.h"
#undef private
#include <memory>
#include "base/test/task_environment.h"
#include "media/base/mock_media_log.h"
#include "media/base/test_helpers.h"

namespace media {

class MockClient : public DemuxerManager::Client {
 public:
  MOCK_METHOD(void,
              OnEncryptedMediaInitData,
              (EmeInitDataType, const std::vector<uint8_t>&),
              (override));
  MOCK_METHOD(void, OnChunkDemuxerOpened, (ChunkDemuxer*), (override));
  MOCK_METHOD(void, OnProgress, (), (override));
  MOCK_METHOD(bool, IsMediaPlayerRendererClient, (), (override));
  MOCK_METHOD(void, OnError, (media::PipelineStatus), (override));
  MOCK_METHOD(void, StopForDemuxerReset, (), (override));
  MOCK_METHOD(void, RestartForHls, (), (override));
#if defined(OHOS_CUSTOM_VIDEO_PLAYER)
  MOCK_METHOD(void, RestartForPrimitive, (), (override));
#endif  // OHOS_CUSTOM_VIDEO_PLAYER
  MOCK_METHOD(bool, IsSecurityOriginCryptographic, (), (const, override));
#if BUILDFLAG(ENABLE_FFMPEG)
  MOCK_METHOD(
      void,
      AddAudioTrack,
      (const std::string&, const std::string&, const std::string&, bool),
      (override));
  MOCK_METHOD(
      void,
      AddVideoTrack,
      (const std::string&, const std::string&, const std::string&, bool),
      (override));
#endif  // BUILDFLAG(ENABLE_FFMPEG)
#if BUILDFLAG(ENABLE_HLS_DEMUXER)
  MOCK_METHOD(base::SequenceBound<HlsDataSourceProvider>,
              GetHlsDataSourceProvider,
              (),
              (override));
#endif  // BUILDFLAG(ENABLE_HLS_DEMUXER)
  MOCK_METHOD(bool, CouldPlayIfEnoughData, (), (override));
  MOCK_METHOD(void, MakeDemuxerThreadDumper, (media::Demuxer*), (override));
  MOCK_METHOD(double, CurrentTime, (), (const, override));
  MOCK_METHOD(void, UpdateLoadedUrl, (const GURL&), (override));
  virtual ~MockClient() = default;
};

class MockDataSource : public DataSource {
 public:
  MOCK_METHOD(void,
              Read,
              (int64_t position,
               int size,
               uint8_t* data,
               media::DataSource::ReadCB read_cb),
              (override));
  MOCK_METHOD(void, Stop, (), (override));
  MOCK_METHOD(void, Abort, (), (override));
  MOCK_METHOD(bool, GetSize, (int64_t * size_out), (override));
  MOCK_METHOD(bool, IsStreaming, (), (override));
  MOCK_METHOD(void, SetBitrate, (int bitrate), (override));
  MOCK_METHOD(bool, PassedTimingAllowOriginCheck, (), (override));
  MOCK_METHOD(bool, WouldTaintOrigin, (), (override));
  MOCK_METHOD(bool, AssumeFullyBuffered, (), (const, override));
  MOCK_METHOD(int64_t, GetMemoryUsage, (), (override));
  MOCK_METHOD(void,
              SetPreload,
              (media::DataSource::Preload preload),
              (override));
  MOCK_METHOD(GURL, GetUrlAfterRedirects, (), (const, override));
  MOCK_METHOD(void,
              OnBufferingHaveEnough,
              (bool must_cancel_netops),
              (override));
  MOCK_METHOD(void,
              OnMediaPlaybackRateChanged,
              (double playback_rate),
              (override));
  MOCK_METHOD(void, OnMediaIsPlaying, (), (override));
  MOCK_METHOD(const media::CrossOriginDataSource*,
              GetAsCrossOriginDataSource,
              (),
              (const, override));
  virtual ~MockDataSource() = default;
};

class DemuxerManagerTest : public testing::Test {
 protected:
  void SetUp() override {
    media_log_ = std::make_unique<MockMediaLog>();
    task_environment_ = std::make_unique<base::test::TaskEnvironment>();
    client_ = std::make_unique<MockClient>();
    data_source_ = std::make_unique<MockDataSource>();
    demuxer_manager_ = std::make_unique<DemuxerManager>(
        client_.get(), task_environment_->GetMainThreadTaskRunner(),
        media_log_.get(), net::SiteForCookies(), url::Origin(), true, false,
        nullptr);
    demuxer_manager_->data_source_.reset(data_source_.release());
  }

  std::unique_ptr<MockMediaLog> media_log_;
  std::unique_ptr<base::test::TaskEnvironment> task_environment_;
  std::unique_ptr<MockClient> client_;
  std::unique_ptr<MockDataSource> data_source_;
  std::unique_ptr<DemuxerManager> demuxer_manager_;
};

TEST_F(DemuxerManagerTest, RestartClientForPrimitive_Client) {
  EXPECT_CALL(*client_, RestartForPrimitive()).Times(1);
  demuxer_manager_->RestartClientForPrimitive();
  EXPECT_TRUE(client_);
}

TEST_F(DemuxerManagerTest, RestartClientForPrimitive_NullClient) {
  demuxer_manager_->client_ = nullptr;
  EXPECT_CALL(*client_, RestartForPrimitive()).Times(0);
  demuxer_manager_->RestartClientForPrimitive();
  EXPECT_FALSE(demuxer_manager_->client_);
}

TEST_F(DemuxerManagerTest, OnPipelineError_Error) {
  EXPECT_CALL(*client_, StopForDemuxerReset()).Times(testing::AtLeast(1));
  EXPECT_CALL(
      *static_cast<MockDataSource*>(demuxer_manager_->data_source_.get()),
      Stop())
      .Times(testing::AtLeast(1));
  demuxer_manager_->OnPipelineError(
      PIPELINE_ERROR_INITIALIZATION_FAILED_CUSTOM_PLAYER);
  EXPECT_TRUE(client_);
  EXPECT_EQ(demuxer_manager_->data_source_, nullptr);
}

TEST_F(DemuxerManagerTest, OnPipelineError_Client) {
  auto data_source_ = std::make_unique<MockDataSource>();
  demuxer_manager_->data_source_ = nullptr;
  EXPECT_CALL(*client_, StopForDemuxerReset()).Times(testing::AtLeast(1));
  EXPECT_CALL(*data_source_, Stop()).Times(testing::AtLeast(0));
  demuxer_manager_->OnPipelineError(
      PIPELINE_ERROR_INITIALIZATION_FAILED_CUSTOM_PLAYER);
  EXPECT_TRUE(client_);
  EXPECT_FALSE(demuxer_manager_->data_source_);
}

TEST_F(DemuxerManagerTest, OnPipelineError_Data_source) {
  demuxer_manager_->client_ = nullptr;
  EXPECT_CALL(*client_, StopForDemuxerReset()).Times(testing::AtLeast(0));
  EXPECT_CALL(
      *static_cast<MockDataSource*>(demuxer_manager_->data_source_.get()),
      Stop())
      .Times(testing::AtLeast(1));
  demuxer_manager_->OnPipelineError(
      PIPELINE_ERROR_INITIALIZATION_FAILED_CUSTOM_PLAYER);
  EXPECT_EQ(demuxer_manager_->data_source_, nullptr);
  EXPECT_FALSE(demuxer_manager_->client_);
}

TEST_F(DemuxerManagerTest, OnPipelineError_LOG) {
  PipelineStatus error = PIPELINE_ERROR_INITIALIZATION_FAILED;
  testing::internal::CaptureStdout();
  demuxer_manager_->OnPipelineError(error);
  std::string output = testing::internal::GetCapturedStdout();
  EXPECT_EQ(output.find("OhMedia::OnError PipelineStatus = 6"),
            std::string::npos);
}
}  // namespace media
