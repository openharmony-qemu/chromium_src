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

#if BUILDFLAG(ARKWEB_CUSTOM_VIDEO_PLAYER)
TEST_P(HTMLVideoElementTest, TestRequestEnterFullscreen) {
  video()->SetSrc(AtomicString("http://example.com/foo.mp4"));
  test::RunPendingTasks();
  UpdateAllLifecyclePhasesForTest();
  EXPECT_FALSE(video()->IsFullscreen());
  ASSERT_NO_FATAL_FAILURE(video()->RequestFullscreen());
}

TEST_P(HTMLVideoElementTest, TestRequestExitFullscreen) {
  video()->SetSrc(AtomicString("http://example.com/foo.mp4"));
  test::RunPendingTasks();
  UpdateAllLifecyclePhasesForTest();
  EXPECT_FALSE(video()->IsFullscreen());
  ASSERT_NO_FATAL_FAILURE(video()->RequestExitFullscreen());
}

#endif  // ARKWEB_CUSTOM_VIDEO_PLAYER

#if BUILDFLAG(ARKWEB_VIDEO_ASSISTANT)
TEST_P(HTMLVideoElementTest, TestSetPlaybackRate) {
  video()->SetSrc(AtomicString("http://example.com/foo.mp4"));
  test::RunPendingTasks();
  UpdateAllLifecyclePhasesForTest();
  double playback_rate = 1.01;
  ASSERT_NO_FATAL_FAILURE(video()->SetPlaybackRate(playback_rate));
}

TEST_P(HTMLVideoElementTest, TestRequestDownloadUrl) {
  video()->SetSrc(AtomicString("http://example.com/foo.mp4"));
  test::RunPendingTasks();
  UpdateAllLifecyclePhasesForTest();
  ASSERT_NO_FATAL_FAILURE(video()->RequestDownloadUrl());
}
#endif  // ARKWEB_VIDEO_ASSISTANT

#if BUILDFLAG(ARKWEB_PIP)
TEST_P(HTMLVideoElementTest, TestOnPictureInPictureStateChanged) {
  video()->SetSrc(AtomicString("http://example.com/foo.mp4"));
  test::RunPendingTasks();
  UpdateAllLifecyclePhasesForTest();
  uint32_t state = 0;
  int32_t width = 1;
  int32_t height = 1;
  ASSERT_NO_FATAL_FAILURE(video()->OnPictureInPictureStateChanged(state, width, height));
}

TEST_P(HTMLVideoElementTest, TestPipDown) {
  video()->SetSrc(AtomicString("http://example.com/foo.mp4"));
  test::RunPendingTasks();
  UpdateAllLifecyclePhasesForTest();
  ASSERT_NO_FATAL_FAILURE(video()->PipDown(true));
  ASSERT_NO_FATAL_FAILURE(video()->PipDown(false));
}

TEST_P(HTMLVideoElementTest, TestRequestExitPictureInPicture) {
  video()->SetSrc(AtomicString("http://example.com/foo.mp4"));
  test::RunPendingTasks();
  UpdateAllLifecyclePhasesForTest();
  ASSERT_NO_FATAL_FAILURE(video()->RequestExitPictureInPicture());
}

TEST_P(HTMLVideoElementTest, TestNotifyPipResize) {
  video()->SetSrc(AtomicString("http://example.com/foo.mp4"));
  test::RunPendingTasks();
  UpdateAllLifecyclePhasesForTest();
  ASSERT_NO_FATAL_FAILURE(video()->NotifyPipResize());
}

TEST_P(HTMLVideoElementTest, TestPipRequestPlay) {
  video()->SetSrc(AtomicString("http://example.com/foo.mp4"));
  test::RunPendingTasks();
  UpdateAllLifecyclePhasesForTest();
  ASSERT_NO_FATAL_FAILURE(video()->PipRequestPlay());
  video()->paused_ = false;
  ASSERT_NO_FATAL_FAILURE(video()->PipRequestPlay());
}
#endif