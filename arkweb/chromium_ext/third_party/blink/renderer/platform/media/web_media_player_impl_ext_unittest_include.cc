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

namespace blink {

#if BUILDFLAG(ARKWEB_CUSTOM_VIDEO_PLAYER)
TEST_F(WebMediaPlayerImplTest, ExtTestOnNativeTextureCreatedWithNullBridge) {
  InitializeWebMediaPlayerImpl();
  int test_native_texture_id = 1;
  bool callback_called = false;
  gfx::Rect received_rect;
  media::Renderer::OnGetRectCallback test_callback = base::BindRepeating(
      [](bool* called, gfx::Rect* rect, const gfx::Rect& r) {},
      &callback_called, &received_rect);
  wmpi_->AsWebMediaPlayerImplExt()->OnNativeTextureCreated(
      test_native_texture_id, test_callback);
  EXPECT_EQ(wmpi_->native_texture_id_, test_native_texture_id);
}

TEST_F(WebMediaPlayerImplTest, ExtTestOnNativeTextureCreatedWithBridge) {
  InitializeWebMediaPlayerImpl();
  int test_native_texture_id = 1;
  bool callback_called = false;
  gfx::Rect received_rect;
  media::Renderer::OnGetRectCallback test_callback = base::BindRepeating(
      [](bool* called, gfx::Rect* rect, const gfx::Rect& r) {},
      &callback_called, &received_rect);

  auto surface_layer_bridge =
      std::make_unique<NiceMock<MockSurfaceLayerBridge>>();
  auto* surface_layer_bridge_ptr = surface_layer_bridge.get();
  scoped_refptr<cc::Layer> layer = cc::Layer::Create();
  EXPECT_CALL(*surface_layer_bridge, GetSurfaceId())
      .WillRepeatedly(ReturnRef(surface_id_));
  EXPECT_CALL(*surface_layer_bridge, GetCcLayer())
      .WillRepeatedly(Return(layer.get()));
  wmpi_->bridge_ = std::move(surface_layer_bridge);
  wmpi_->AsWebMediaPlayerImplExt()->OnNativeTextureCreated(
      test_native_texture_id, test_callback);
  EXPECT_EQ(wmpi_->native_texture_id_, test_native_texture_id);
}

TEST_F(WebMediaPlayerImplTest, ExtTestDoReloadForRemotingScheme1) {
  auto mock_demuxer = std::make_unique<NiceMock<media::MockDemuxer>>();
  InitializeWebMediaPlayerImpl(std::move(mock_demuxer));
  ASSERT_NO_FATAL_FAILURE(
      wmpi_->AsWebMediaPlayerImplExt()->DoReloadForPrimitive());
  EXPECT_EQ(wmpi_->demuxer_manager_->GetDataSourceForTesting(), nullptr);
}

TEST_F(WebMediaPlayerImplTest, ExtTestDoReloadForRemotingScheme2) {
  InitializeWebMediaPlayerImpl();
  wmpi_->demuxer_manager_->SetLoadedUrl(GURL("media-remoting:test-video"));
  auto data_source = std::make_unique<media::MemoryDataSource>("test data");
  wmpi_->demuxer_manager_->data_source_ = std::move(data_source);
  ASSERT_NO_FATAL_FAILURE(
      wmpi_->AsWebMediaPlayerImplExt()->DoReloadForPrimitive());
}

TEST_F(WebMediaPlayerImplTest, ExtTestDoReloadForRemotingScheme3) {
  InitializeWebMediaPlayerImpl();
  SetLoadType(WebMediaPlayer::kLoadTypeMediaSource);
  EXPECT_EQ(wmpi_->load_type_, WebMediaPlayer::kLoadTypeMediaSource);
  ASSERT_NO_FATAL_FAILURE(
      wmpi_->AsWebMediaPlayerImplExt()->DoReloadForPrimitive());
}

TEST_F(WebMediaPlayerImplTest, ExtTestDoReloadForRemotingScheme4) {
  auto mock_demuxer = std::make_unique<NiceMock<media::MockDemuxer>>();
  InitializeWebMediaPlayerImpl(std::move(mock_demuxer));
  SetLoadType(WebMediaPlayer::kLoadTypeMediaSource);
  ASSERT_NO_FATAL_FAILURE(
      wmpi_->AsWebMediaPlayerImplExt()->DoReloadForPrimitive());
  EXPECT_EQ(wmpi_->demuxer_manager_->GetDataSourceForTesting(), nullptr);
}

TEST_F(WebMediaPlayerImplTest, ExtTestDoReloadForRemotingScheme5) {
  auto mock_demuxer = std::make_unique<NiceMock<media::MockDemuxer>>();
  InitializeWebMediaPlayerImpl(std::move(mock_demuxer));
  wmpi_->demuxer_manager_->SetLoadedUrl(GURL("media-remoting:test-video"));
  auto data_source = std::make_unique<media::MemoryDataSource>("test data");
  wmpi_->demuxer_manager_->data_source_ = std::move(data_source);
  ASSERT_NO_FATAL_FAILURE(
      wmpi_->AsWebMediaPlayerImplExt()->DoReloadForPrimitive());
  EXPECT_NE(wmpi_->demuxer_manager_->GetDataSourceForTesting(), nullptr);
}

TEST_F(WebMediaPlayerImplTest, ExtTestDoReloadForRemotingScheme6) {
  InitializeWebMediaPlayerImpl();
  SetLoadType(WebMediaPlayer::kLoadTypeMediaSource);
  wmpi_->demuxer_manager_->SetLoadedUrl(GURL("media-remoting:test-video"));
  auto data_source = std::make_unique<media::MemoryDataSource>("test data");
  wmpi_->demuxer_manager_->data_source_ = std::move(data_source);
  ASSERT_NO_FATAL_FAILURE(
      wmpi_->AsWebMediaPlayerImplExt()->DoReloadForPrimitive());
  EXPECT_NE(wmpi_->demuxer_manager_->GetDataSourceForTesting(), nullptr);
}

TEST_F(WebMediaPlayerImplTest, ExtTestDoReloadForDataScheme1) {
  InitializeWebMediaPlayerImpl();
  wmpi_->demuxer_manager_->SetLoadedUrl(
      GURL("data:text/plain;base64,SGVsbG8sIFdvcmxkIQ=="));
  ASSERT_NO_FATAL_FAILURE(
      wmpi_->AsWebMediaPlayerImplExt()->DoReloadForPrimitive());
  EXPECT_NE(wmpi_->demuxer_manager_->GetDataSourceForTesting(), nullptr);
}

TEST_F(WebMediaPlayerImplTest, ExtTestDoReloadForDataScheme2) {
  InitializeWebMediaPlayerImpl();
  wmpi_->demuxer_manager_->SetLoadedUrl(GURL("data:"));
  ASSERT_NO_FATAL_FAILURE(
      wmpi_->AsWebMediaPlayerImplExt()->DoReloadForPrimitive());
  EXPECT_EQ(wmpi_->demuxer_manager_->GetDataSourceForTesting(), nullptr);
}

TEST_F(WebMediaPlayerImplTest, ExtTestDoReloadForDataScheme4) {
  InitializeWebMediaPlayerImpl();
  wmpi_->demuxer_manager_->SetLoadedUrl(GURL("data:,"));
  ASSERT_NO_FATAL_FAILURE(
      wmpi_->AsWebMediaPlayerImplExt()->DoReloadForPrimitive());
  EXPECT_EQ(wmpi_->demuxer_manager_->GetDataSourceForTesting(), nullptr);
}

TEST_F(WebMediaPlayerImplTest, ExtTestOnLayerRectChange) {
  InitializeWebMediaPlayerImpl();
  int test_native_texture_id = 1;
  bool callback_called = false;
  gfx::Rect received_rect;
  ASSERT_NO_FATAL_FAILURE(
      wmpi_->AsWebMediaPlayerImplExt()->OnLayerRectChange(received_rect));

  media::Renderer::OnGetRectCallback test_callback = base::BindRepeating(
      [](bool* called, gfx::Rect* rect, const gfx::Rect& r) {},
      &callback_called, &received_rect);

  wmpi_->AsWebMediaPlayerImplExt()->OnNativeTextureCreated(
      test_native_texture_id, test_callback);
  EXPECT_EQ(wmpi_->native_texture_id_, test_native_texture_id);
  ASSERT_NO_FATAL_FAILURE(
      wmpi_->AsWebMediaPlayerImplExt()->OnLayerRectChange(received_rect));
}

TEST_F(WebMediaPlayerImplTest, ExtTestOnSurfaceRequested) {
  InitializeWebMediaPlayerImpl();
  bool callback_called = false;
  media::SurfaceCreatedCB surface_callback = base::BindRepeating(
      [](bool* called, int widget_id) { *called = true; }, &callback_called);
  ASSERT_NO_FATAL_FAILURE(wmpi_->AsWebMediaPlayerImplExt()->SetVideoSurface(1));
  ASSERT_NO_FATAL_FAILURE(wmpi_->AsWebMediaPlayerImplExt()->OnSurfaceRequested(
      surface_callback, true, ""));
  EXPECT_TRUE(callback_called);
  callback_called = false;
  ASSERT_NO_FATAL_FAILURE(wmpi_->AsWebMediaPlayerImplExt()->SetVideoSurface(0));
  ASSERT_NO_FATAL_FAILURE(wmpi_->AsWebMediaPlayerImplExt()->OnSurfaceRequested(
      surface_callback, true, ""));
  EXPECT_TRUE(callback_called);
}

TEST_F(WebMediaPlayerImplTest, ExtTestSetVideoSurface) {
  InitializeWebMediaPlayerImpl();
  SetPaused(true);
  SetSeeking(true);
  ASSERT_NO_FATAL_FAILURE(wmpi_->AsWebMediaPlayerImplExt()->SetVideoSurface(1));
  EXPECT_EQ(wmpi_->video_surface_id_, 1);

  SetPaused(true);
  SetSeeking(false);
  ASSERT_NO_FATAL_FAILURE(wmpi_->AsWebMediaPlayerImplExt()->SetVideoSurface(2));
  EXPECT_EQ(wmpi_->video_surface_id_, 2);

  SetPaused(false);
  SetSeeking(true);
  ASSERT_NO_FATAL_FAILURE(wmpi_->AsWebMediaPlayerImplExt()->SetVideoSurface(3));
  EXPECT_EQ(wmpi_->video_surface_id_, 3);

  SetPaused(false);
  SetSeeking(false);
  ASSERT_NO_FATAL_FAILURE(wmpi_->AsWebMediaPlayerImplExt()->SetVideoSurface(4));
  EXPECT_EQ(wmpi_->video_surface_id_, 4);
}
#endif  // ARKWEB_CUSTOM_VIDEO_PLAYER

#if BUILDFLAG(ARKWEB_PIP)
TEST_F(WebMediaPlayerImplTest, ExtTestPipEnable) {
  InitializeWebMediaPlayerImpl();
  ASSERT_NO_FATAL_FAILURE(wmpi_->AsWebMediaPlayerImplExt()->PipEnable(false));
  auto& original_pipeline_controller_ref = wmpi_->pipeline_controller_;
  auto original_pipeline_controller =
      std::move(original_pipeline_controller_ref);
  std::unique_ptr<media::PipelineController> pipeline_controller = nullptr;
  wmpi_->pipeline_controller_ = std::move(pipeline_controller);
  ASSERT_NO_FATAL_FAILURE(wmpi_->AsWebMediaPlayerImplExt()->PipEnable(false));
  wmpi_->pipeline_controller_ = std::move(original_pipeline_controller);
}
#endif  // ARKWEB_PIP

#if BUILDFLAG(ARKWEB_MEDIA_DMABUF)
TEST_F(WebMediaPlayerImplTest, ExtTestRecycleDmaBuffer1) {
  InitializeWebMediaPlayerImpl();

  delegate_.SetPageHiddenForTesting(true);
  delegate_.SetFrameHiddenForTesting(true);
  wmpi_->should_pause_when_frame_is_hidden_ = true;
  ASSERT_NO_FATAL_FAILURE(wmpi_->AsWebMediaPlayerImplExt()->RecycleDmaBuffer());

  delegate_.SetPageHiddenForTesting(true);
  delegate_.SetFrameHiddenForTesting(false);
  wmpi_->should_pause_when_frame_is_hidden_ = false;
  ASSERT_NO_FATAL_FAILURE(wmpi_->AsWebMediaPlayerImplExt()->RecycleDmaBuffer());

  delegate_.SetPageHiddenForTesting(true);
  delegate_.SetFrameHiddenForTesting(true);
  wmpi_->should_pause_when_frame_is_hidden_ = false;
  ASSERT_NO_FATAL_FAILURE(wmpi_->AsWebMediaPlayerImplExt()->RecycleDmaBuffer());

  delegate_.SetPageHiddenForTesting(true);
  delegate_.SetFrameHiddenForTesting(false);
  wmpi_->should_pause_when_frame_is_hidden_ = true;
  ASSERT_NO_FATAL_FAILURE(wmpi_->AsWebMediaPlayerImplExt()->RecycleDmaBuffer());

  delegate_.SetPageHiddenForTesting(false);
  delegate_.SetFrameHiddenForTesting(true);
  wmpi_->should_pause_when_frame_is_hidden_ = true;
  ASSERT_NO_FATAL_FAILURE(wmpi_->AsWebMediaPlayerImplExt()->RecycleDmaBuffer());

  delegate_.SetPageHiddenForTesting(false);
  delegate_.SetFrameHiddenForTesting(true);
  wmpi_->should_pause_when_frame_is_hidden_ = false;
  ASSERT_NO_FATAL_FAILURE(wmpi_->AsWebMediaPlayerImplExt()->RecycleDmaBuffer());

  delegate_.SetPageHiddenForTesting(false);
  delegate_.SetFrameHiddenForTesting(false);
  wmpi_->should_pause_when_frame_is_hidden_ = true;
  ASSERT_NO_FATAL_FAILURE(wmpi_->AsWebMediaPlayerImplExt()->RecycleDmaBuffer());

  delegate_.SetPageHiddenForTesting(false);
  delegate_.SetFrameHiddenForTesting(false);
  wmpi_->should_pause_when_frame_is_hidden_ = false;
  ASSERT_NO_FATAL_FAILURE(wmpi_->AsWebMediaPlayerImplExt()->RecycleDmaBuffer());
}

TEST_F(WebMediaPlayerImplTest, ExtTestRecycleDmaBuffer2) {
  InitializeWebMediaPlayerImpl();
  delegate_.SetPageHiddenForTesting(true);
  ASSERT_NO_FATAL_FAILURE(wmpi_->AsWebMediaPlayerImplExt()->RecycleDmaBuffer());
  wmpi_->is_flinging_ = true;
  ASSERT_NO_FATAL_FAILURE(wmpi_->AsWebMediaPlayerImplExt()->RecycleDmaBuffer());
}

TEST_F(WebMediaPlayerImplTest, ExtTestRecycleDmaBuffer3) {
  InitializeWebMediaPlayerImpl();
  delegate_.SetPageHiddenForTesting(false);
  SetPaused(true);
  ASSERT_NO_FATAL_FAILURE(wmpi_->AsWebMediaPlayerImplExt()->RecycleDmaBuffer());
  SetPaused(false);
  ASSERT_NO_FATAL_FAILURE(wmpi_->AsWebMediaPlayerImplExt()->RecycleDmaBuffer());
}

TEST_F(WebMediaPlayerImplTest, ExtTestIsDmaBufferRecycleEnabled) {
  InitializeWebMediaPlayerImpl();
  wmpi_->AsWebMediaPlayerImplExt()->SetDmaBufferSeekState(true);
  EXPECT_TRUE(wmpi_->AsWebMediaPlayerImplExt()->IsDmaBufferRecycleEnabled());
  wmpi_->AsWebMediaPlayerImplExt()->SetDmaBufferSeekState(false);
  EXPECT_FALSE(wmpi_->AsWebMediaPlayerImplExt()->IsDmaBufferRecycleEnabled());
}

TEST_F(WebMediaPlayerImplTest, ExtTestResumeDmaBuffer1) {
  InitializeWebMediaPlayerImpl();
  auto& original_pipeline_controller_ref = wmpi_->pipeline_controller_;
  auto original_pipeline_controller =
      std::move(original_pipeline_controller_ref);
  std::unique_ptr<media::PipelineController> pipeline_controller = nullptr;
  wmpi_->pipeline_controller_ = std::move(pipeline_controller);
  ASSERT_NO_FATAL_FAILURE(wmpi_->AsWebMediaPlayerImplExt()->ResumeDmaBuffer());
  wmpi_->pipeline_controller_ = std::move(original_pipeline_controller);
  EXPECT_EQ(wmpi_->AsWebMediaPlayerImplExt()->dma_state_,
            WebMediaPlayerImpl::kHaveExist);
}

TEST_F(WebMediaPlayerImplTest, ExtTestResumeDmaBuffer2) {
  InitializeWebMediaPlayerImpl();
  wmpi_->AsWebMediaPlayerImplExt()->dma_state_ = WebMediaPlayerImpl::kHaveExist;
  ASSERT_NO_FATAL_FAILURE(wmpi_->AsWebMediaPlayerImplExt()->ResumeDmaBuffer());

  wmpi_->AsWebMediaPlayerImplExt()->dma_state_ =
      WebMediaPlayerImpl::kHaveRecycled;
  ASSERT_NO_FATAL_FAILURE(wmpi_->AsWebMediaPlayerImplExt()->ResumeDmaBuffer());
  EXPECT_EQ(wmpi_->AsWebMediaPlayerImplExt()->dma_state_,
            WebMediaPlayerImpl::kHaveExist);
}
#endif  // ARKWEB_MEDIA_DMABUF

#if BUILDFLAG(ARKWEB_BFCACHE)
TEST_F(WebMediaPlayerImplTest, ExtTestMediaResumeFromBFCachePage) {
  InitializeWebMediaPlayerImpl();
  EXPECT_CALL(client_, ResumePlayback()).Times(3);

  EXPECT_CALL(client_, IsMediaResumeFromBFCachePage())
      .WillRepeatedly(Return(false));
  ASSERT_NO_FATAL_FAILURE(
      wmpi_->AsWebMediaPlayerImplExt()->MediaResumeFromBFCachePage(true));

  EXPECT_CALL(client_, IsMediaResumeFromBFCachePage())
      .WillRepeatedly(Return(true));
  ASSERT_NO_FATAL_FAILURE(
      wmpi_->AsWebMediaPlayerImplExt()->MediaResumeFromBFCachePage(true));

  EXPECT_CALL(client_, IsMediaResumeFromBFCachePage())
      .WillRepeatedly(Return(false));
  ASSERT_NO_FATAL_FAILURE(
      wmpi_->AsWebMediaPlayerImplExt()->MediaResumeFromBFCachePage(false));

  EXPECT_CALL(client_, IsMediaResumeFromBFCachePage())
      .WillRepeatedly(Return(true));
  ASSERT_NO_FATAL_FAILURE(
      wmpi_->AsWebMediaPlayerImplExt()->MediaResumeFromBFCachePage(false));
}
#endif  // BUILDFLAG(ARKWEB_BFCACHE)

#if BUILDFLAG(ARKWEB_MEDIA_CAPABILITIES_ENHANCE)
TEST_F(WebMediaPlayerImplTest, ExtTestGetMimeType) {
  InitializeWebMediaPlayerImpl();
  wmpi_->demuxer_manager_->SetLoadedUrl(GURL("media-remoting:test-video"));
  auto data_source = std::make_unique<media::MemoryDataSource>("test data");
  wmpi_->demuxer_manager_->data_source_ = std::move(data_source);
  ASSERT_NO_FATAL_FAILURE(
      wmpi_->AsWebMediaPlayerImplExt()->GetMimeType());
}

TEST_F(WebMediaPlayerImplTest, ExtTestUsingMediaPlayer) {
  InitializeWebMediaPlayerImpl();
  ASSERT_NO_FATAL_FAILURE(wmpi_->AsWebMediaPlayerImplExt()->UsingMediaPlayer());
}
#endif // ARKWEB_MEDIA_CAPABILITIES_ENHANCE

}  // namespace blink