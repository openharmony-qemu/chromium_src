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

#if BUILDFLAG(ARKWEB_MEDIA)
TEST_F(MojoRendererTest, SetNativeWindowSurface) {
  Initialize();
  int native_window_id = 456;
  ASSERT_NO_FATAL_FAILURE(mojo_renderer_->SetNativeWindowSurface(native_window_id));
}
#endif

#if BUILDFLAG(ARKWEB_CUSTOM_VIDEO_PLAYER)
TEST_F(MojoRendererTest, SetMuted) {
  bool muted_ = true;
  mojo_renderer_->SetMuted(muted_);
  EXPECT_EQ(muted_, mojo_renderer_->muted_);
  Destroy();
}

TEST_F(MojoRendererTest, SetSurfaceId) {
  const gfx::Rect test_rect(10, 20, 30, 40);
  int surface_id = 5;
  ASSERT_NO_FATAL_FAILURE(mojo_renderer_->SetSurfaceId(surface_id, test_rect));
  Destroy();
}

TEST_F(MojoRendererTest, SetMediaSourceList) {
  std::vector <media::Renderer::MediaSourceInfo> source_infos;
  media::Renderer::MediaSourceInfo info;
  info.media_format = "video/mp4";
  info.media_source = "http://proxy1.com";
  source_infos.emplace_back(std::move(info));
  mojo_renderer_->SetMediaSourceList(source_infos);
  EXPECT_FALSE(mojo_renderer_->source_infos_.empty());
  Destroy();
}

TEST_F(MojoRendererTest, SetMediaControls) {
  std::vector<std::string> controls_list;
  bool show_media_controls = true;
  mojo_renderer_->SetMediaControls(show_media_controls, controls_list);
  EXPECT_TRUE(show_media_controls == mojo_renderer_->show_media_controls_);
  Destroy();
}

TEST_F(MojoRendererTest, SetPoster) {
  std::string url = "http://proxy1.com";
  mojo_renderer_->SetPoster(url);
  EXPECT_EQ(mojo_renderer_->poster_url_, url);
  Destroy();
}

TEST_F(MojoRendererTest, SetAttributes) {
  base::flat_map<std::string, std::string> attributes;
  attributes["preload"] = "metadata";
  mojo_renderer_->SetAttributes(attributes);
  EXPECT_FALSE(mojo_renderer_->attributes_.empty());
  Destroy();
}

TEST_F(MojoRendererTest, SetReferrer) {
  std::string referrer = "https://example.com/video.html";
  mojo_renderer_->SetReferrer(referrer);
  EXPECT_EQ(mojo_renderer_->referrer_, referrer);
  Destroy();
}

TEST_F(MojoRendererTest, IsAudio) {
  bool audio = true;
  mojo_renderer_->SetIsAudio(audio);
  EXPECT_TRUE(mojo_renderer_->IsAudio());
  Destroy();
}

#endif

#if BUILDFLAG(ARKWEB_VIDEO_ASSISTANT)
TEST_F(MojoRendererTest, OnRequestVideoSurfaceDone) {
  Initialize();
  int32_t surface_id = 1;
  ASSERT_NO_FATAL_FAILURE(mojo_renderer_->OnRequestVideoSurfaceDone(surface_id));
  Destroy();
}
#endif

#if BUILDFLAG(ARKWEB_PIP)
TEST_F(MojoRendererTest, PipEnable) {
  Initialize();
  bool enable = true;
  EXPECT_CALL(*mock_renderer_, PipEnable(enable));
  ASSERT_NO_FATAL_FAILURE(mojo_renderer_->PipEnable(enable));
  Destroy();
}
#endif

#if BUILDFLAG(ARKWEB_MEDIA_DMABUF)
TEST_F(MojoRendererTest, RecycleDmaBuffer001) {
  Initialize();
  mojo_renderer_->remote_renderer_.reset();
  EXPECT_CALL(*mock_renderer_, RecycleDmaBuffer()).Times(0);
  EXPECT_NO_FATAL_FAILURE(mojo_renderer_->RecycleDmaBuffer());
  Destroy();
}

TEST_F(MojoRendererTest, RecycleDmaBuffer002) {
  Initialize();
  EXPECT_CALL(*mock_renderer_, RecycleDmaBuffer()).Times(1);
  EXPECT_NO_FATAL_FAILURE(mojo_renderer_->RecycleDmaBuffer());
  Destroy();
}

TEST_F(MojoRendererTest, ResumeDmaBuffer001) {
  Initialize();
  mojo_renderer_->remote_renderer_.reset();
  EXPECT_CALL(*mock_renderer_, ResumeDmaBuffer()).Times(0);
  EXPECT_NO_FATAL_FAILURE(mojo_renderer_->ResumeDmaBuffer());
  Destroy();
}

TEST_F(MojoRendererTest, ResumeDmaBuffer002) {
  Initialize();
  EXPECT_CALL(*mock_renderer_, ResumeDmaBuffer()).Times(1);
  EXPECT_NO_FATAL_FAILURE(mojo_renderer_->ResumeDmaBuffer());
  Destroy();
}
#endif
