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
TEST_F(RendererImplTest, TestPipEnable) {
  InitializeWithAudio();
  Play();
  ASSERT_NO_FATAL_FAILURE(renderer_impl_->PipEnable(true));
  renderer_impl_->video_renderer_ = nullptr;
  ASSERT_NO_FATAL_FAILURE(renderer_impl_->PipEnable(true));
}
#endif

#if BUILDFLAG(ARKWEB_MEDIA_DMABUF)
TEST_F(RendererImplTest, TestRecycleDmaBuffer) {
  InitializeWithAudio();
  Play();
  ASSERT_NO_FATAL_FAILURE(renderer_impl_->RecycleDmaBuffer());
  renderer_impl_->video_renderer_ = nullptr;
  ASSERT_NO_FATAL_FAILURE(renderer_impl_->RecycleDmaBuffer());
}

TEST_F(RendererImplTest, TestResumeDmaBuffer) {
  InitializeWithAudio();
  Play();
  ASSERT_NO_FATAL_FAILURE(renderer_impl_->ResumeDmaBuffer());
  renderer_impl_->video_renderer_ = nullptr;
  ASSERT_NO_FATAL_FAILURE(renderer_impl_->ResumeDmaBuffer());
}
#endif  // ARKWEB_MEDIA_DMABUF