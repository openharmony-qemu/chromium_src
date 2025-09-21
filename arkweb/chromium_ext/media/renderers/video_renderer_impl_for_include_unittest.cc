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

#if BUILDFLAG(ARKWEB_PIP)
TEST_F(VideoRendererImplTest, VideoRendererImplIncludePipEnable) {
  Initialize();
  StartPlayingFrom(0);
  ASSERT_NO_FATAL_FAILURE(renderer_->PipEnable(true));
  renderer_->video_decoder_stream_ = nullptr;
  ASSERT_NO_FATAL_FAILURE(renderer_->PipEnable(true));
}
#endif // ARKWEB_PIP

#if BUILDFLAG(ARKWEB_MEDIA_DMABUF)
TEST_F(VideoRendererImplTest, VideoRendererImplIncludeRecycleDmaBuffer) {
  Initialize();
  StartPlayingFrom(0);
  ASSERT_NO_FATAL_FAILURE(renderer_->RecycleDmaBuffer());
  renderer_->video_decoder_stream_ = nullptr;
  ASSERT_NO_FATAL_FAILURE(renderer_->RecycleDmaBuffer());
}

TEST_F(VideoRendererImplTest, VideoRendererImplIncludeResumeDmaBuffer) {
  Initialize();
  StartPlayingFrom(0);
  ASSERT_NO_FATAL_FAILURE(renderer_->ResumeDmaBuffer());
  renderer_->video_decoder_stream_ = nullptr;
  ASSERT_NO_FATAL_FAILURE(renderer_->ResumeDmaBuffer());
}
#endif // ARKWEB_MEDIA_DMABUF