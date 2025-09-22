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

#if BUILDFLAG(ARKWEB_WEBRTC)
TEST_F(VideoCaptureManagerTest, TestExtStartCamera) {
  int nWebId = 0;
  media::VideoCaptureSessionId session_id_1 = base::UnguessableToken::Create();
  vcm_->AsVideoCaptureManagerExt()->BindSessionIdToNWebId(session_id_1, nWebId);
  ASSERT_NO_FATAL_FAILURE(vcm_->AsVideoCaptureManagerExt()->StartCamera(0));
  vcm_->AsVideoCaptureManagerExt()->BindSessionIdToNWebId(session_id_1, nWebId);
  ASSERT_NO_FATAL_FAILURE(vcm_->AsVideoCaptureManagerExt()->StartCamera(1));
  vcm_->AsVideoCaptureManagerExt()->nWebId_ = {};
  ASSERT_NO_FATAL_FAILURE(vcm_->AsVideoCaptureManagerExt()->StartCamera(0));
  // Wait to check callbacks before removing the listener.
  base::RunLoop().RunUntilIdle();
  vcm_->UnregisterListener(listener_.get());
}

TEST_F(VideoCaptureManagerTest, TestExtStopCamera) {
  int nWebId = 0;
  media::VideoCaptureSessionId session_id_1 = base::UnguessableToken::Create();
  vcm_->AsVideoCaptureManagerExt()->BindSessionIdToNWebId(session_id_1, nWebId);
  ASSERT_NO_FATAL_FAILURE(vcm_->AsVideoCaptureManagerExt()->StopCamera(0));
  vcm_->AsVideoCaptureManagerExt()->BindSessionIdToNWebId(session_id_1, nWebId);
  ASSERT_NO_FATAL_FAILURE(vcm_->AsVideoCaptureManagerExt()->StopCamera(1));
  vcm_->AsVideoCaptureManagerExt()->nWebId_ = {};
  ASSERT_NO_FATAL_FAILURE(vcm_->AsVideoCaptureManagerExt()->StopCamera(0));

  // Wait to check callbacks before removing the listener.
  base::RunLoop().RunUntilIdle();
  vcm_->UnregisterListener(listener_.get());
}

TEST_F(VideoCaptureManagerTest, TestExtCloseCamera) {
  int nWebId = 0;
  media::VideoCaptureSessionId session_id_1 = base::UnguessableToken::Create();
  vcm_->AsVideoCaptureManagerExt()->BindSessionIdToNWebId(session_id_1, nWebId);
  ASSERT_NO_FATAL_FAILURE(vcm_->AsVideoCaptureManagerExt()->CloseCamera(0));
  vcm_->AsVideoCaptureManagerExt()->BindSessionIdToNWebId(session_id_1, nWebId);
  ASSERT_NO_FATAL_FAILURE(vcm_->AsVideoCaptureManagerExt()->CloseCamera(1));
  vcm_->AsVideoCaptureManagerExt()->nWebId_ = {};
  ASSERT_NO_FATAL_FAILURE(vcm_->AsVideoCaptureManagerExt()->CloseCamera(0));

  // Wait to check callbacks before removing the listener.
  base::RunLoop().RunUntilIdle();
  vcm_->UnregisterListener(listener_.get());
}
#endif  // BUILDFLAG(ARKWEB_WEBRTC)

#if BUILDFLAG(ARKWEB_EX_SCREEN_CAPTURE)
TEST_F(VideoCaptureManagerTest, TestExtStopScreenCapture) {
  ASSERT_NO_FATAL_FAILURE(vcm_->AsVideoCaptureManagerExt()->StopScreenCapture(""));
  ASSERT_NO_FATAL_FAILURE(vcm_->AsVideoCaptureManagerExt()->StopScreenCapture("AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"));
  // Wait to check callbacks before removing the listener.
  base::RunLoop().RunUntilIdle();
  vcm_->UnregisterListener(listener_.get());
}

TEST_F(VideoCaptureManagerTest, TestExtOnScreenCaptureOpened) {
  ASSERT_NO_FATAL_FAILURE(vcm_->AsVideoCaptureManagerExt()->OnScreenCaptureOpened(""));
  vcm_->UnregisterListener(listener_.get());
  ASSERT_NO_FATAL_FAILURE(vcm_->AsVideoCaptureManagerExt()->OnScreenCaptureOpened(""));
  // Wait to check callbacks before removing the listener.
  base::RunLoop().RunUntilIdle();
  vcm_->UnregisterListener(listener_.get());
}
#endif  // defined(ARKWEB_EX_SCREEN_CAPTURE)