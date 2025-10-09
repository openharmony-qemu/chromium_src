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

#if BUILDFLAG(ARKWEB_VIDEO_ASSISTANT)
TEST_P(VideoDecoderStreamTest, SetVideoSurface) {
  Initialize();
  EXPECT_NO_FATAL_FAILURE(video_decoder_stream_->SetVideoSurface(1));
}
#endif  // ARKWEB_VIDEO_ASSISTANT

#if BUILDFLAG(ARKWEB_PIP)
TEST_P(VideoDecoderStreamTest, PipEnable) {
  Initialize();
  EXPECT_NO_FATAL_FAILURE(video_decoder_stream_->PipEnable(true));
  EXPECT_NO_FATAL_FAILURE(video_decoder_stream_->PipEnable(false));
}
#endif  // ARKWEB_PIP

#if BUILDFLAG(ARKWEB_MEDIA_DMABUF)
TEST_P(VideoDecoderStreamTest, DmaBufferOperations) {
  Initialize();
  EXPECT_NE(video_decoder_stream_->traits_, nullptr);
  EXPECT_NO_FATAL_FAILURE(video_decoder_stream_->RecycleDmaBuffer());
  EXPECT_NO_FATAL_FAILURE(video_decoder_stream_->ResumeDmaBuffer());
  video_decoder_stream_->traits_ = nullptr;
  EXPECT_EQ(video_decoder_stream_->traits_, nullptr);
  EXPECT_NO_FATAL_FAILURE(video_decoder_stream_->RecycleDmaBuffer());
  EXPECT_NO_FATAL_FAILURE(video_decoder_stream_->ResumeDmaBuffer());
}
#endif  // ARKWEB_VIDEO_ASSISTANT