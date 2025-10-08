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

using ScreenCaptureCallback = base::RepeatingCallback<
      void(int32_t nweb_id, const char* session_id, int32_t code)>;

#if BUILDFLAG(ARKWEB_EX_SCREEN_CAPTURE)   
TEST_F(MediaStreamManagerTest, SetScreenCaptureDelegateCallback001) {
  ScreenCaptureCallback callback;
  ASSERT_TRUE(media_stream_manager_->AsMediaStreamManagerExt());
  media_stream_manager_->AsMediaStreamManagerExt()
      ->SetScreenCaptureDelegateCallback(callback);
  EXPECT_TRUE(media_stream_manager_->AsMediaStreamManagerExt()
                ->screen_capture_callback_.is_null());
}

TEST_F(MediaStreamManagerTest, StopScreenCapture001) {
  media_stream_manager_->AsMediaStreamManagerExt()->video_capture_manager_ =
      nullptr;
  int nweb_id = 1;
  std::string session_id_str = "session_id_str";
  ASSERT_NO_FATAL_FAILURE(
      media_stream_manager_->AsMediaStreamManagerExt()->StopScreenCapture(
          nweb_id, session_id_str));
}

TEST_F(MediaStreamManagerTest, StopScreenCapture002) {
  int nweb_id = 1;
  std::string session_id_str = "session_id_str";
  ASSERT_NO_FATAL_FAILURE(
      media_stream_manager_->AsMediaStreamManagerExt()->StopScreenCapture(
          nweb_id, session_id_str));
}

TEST_F(MediaStreamManagerTest, StopScreenCapture003) {
  int nweb_id = 1;
  std::string session_id_str = "session_id_str";
  media_stream_manager_->AsMediaStreamManagerExt()
      ->nweb_id_maps_[session_id_str] = 0;
  ASSERT_NO_FATAL_FAILURE(
      media_stream_manager_->AsMediaStreamManagerExt()->StopScreenCapture(
          nweb_id, session_id_str));
}

TEST_F(MediaStreamManagerTest, StopScreenCapture004) {
  int nweb_id = 1;
  std::string session_id_str = "session_id_str";
  media_stream_manager_->AsMediaStreamManagerExt()
      ->nweb_id_maps_[session_id_str] = nweb_id;
  ASSERT_NO_FATAL_FAILURE(
      media_stream_manager_->AsMediaStreamManagerExt()->StopScreenCapture(
          nweb_id, session_id_str));
}

TEST_F(MediaStreamManagerTest, SetScreenCapturePickerShow001) {
  media_stream_manager_->AsMediaStreamManagerExt()->video_capture_manager_ =
      nullptr;
  ASSERT_NO_FATAL_FAILURE(media_stream_manager_->AsMediaStreamManagerExt()
                              ->SetScreenCapturePickerShow());
}

TEST_F(MediaStreamManagerTest, SetScreenCapturePickerShow002) {
  ASSERT_NO_FATAL_FAILURE(media_stream_manager_->AsMediaStreamManagerExt()
                              ->SetScreenCapturePickerShow());
}

TEST_F(MediaStreamManagerTest, DisableSessionReuse001) {
  media_stream_manager_->AsMediaStreamManagerExt()->video_capture_manager_ =
      nullptr;
  ASSERT_NO_FATAL_FAILURE(
      media_stream_manager_->AsMediaStreamManagerExt()->DisableSessionReuse());
}

TEST_F(MediaStreamManagerTest, DisableSessionReuse002) {
  ASSERT_NO_FATAL_FAILURE(
      media_stream_manager_->AsMediaStreamManagerExt()->DisableSessionReuse());
}

TEST_F(MediaStreamManagerTest, SendScreenCaptureState001) {
  int nweb_id = 1;
  std::string session_id_str = "session_id_str";
  media_stream_manager_->AsMediaStreamManagerExt()
      ->nweb_id_maps_[session_id_str] = nweb_id;
  ASSERT_NO_FATAL_FAILURE(
      media_stream_manager_->AsMediaStreamManagerExt()->SendScreenCaptureState(
          session_id_str, 0));
}

TEST_F(MediaStreamManagerTest, SendScreenCaptureState002) {
  std::string session_id_str = "session_id_str";
  ASSERT_NO_FATAL_FAILURE(
      media_stream_manager_->AsMediaStreamManagerExt()->SendScreenCaptureState(
          session_id_str, 0));
}

TEST_F(MediaStreamManagerTest, SendScreenCaptureStateToNative001) {
  int nweb_id = 1;
  std::string session_id_str = "session_id_str";
  media_stream_manager_->AsMediaStreamManagerExt()
      ->nweb_id_maps_[session_id_str] = nweb_id;
  ScreenCaptureCallback callback;
  media_stream_manager_->AsMediaStreamManagerExt()->screen_capture_callback_ =
      std::move(callback);
  ASSERT_NO_FATAL_FAILURE(
      media_stream_manager_->AsMediaStreamManagerExt()
          ->SendScreenCaptureStateToNative(nweb_id, session_id_str, 0));
}

TEST_F(MediaStreamManagerTest, SendScreenCaptureStateToNative002) {
  int nweb_id = 1;
  std::string session_id_str = "session_id_str";
  media_stream_manager_->AsMediaStreamManagerExt()
      ->nweb_id_maps_[session_id_str] = nweb_id;
  static int res = 0;
  ScreenCaptureCallback callback = base::BindRepeating(
      [](int32_t nweb_id, const char* session_id, int32_t code) {res = 1;});
  media_stream_manager_->AsMediaStreamManagerExt()->screen_capture_callback_ =
      std::move(callback);
  ASSERT_NO_FATAL_FAILURE(
      media_stream_manager_->AsMediaStreamManagerExt()
          ->SendScreenCaptureStateToNative(nweb_id, session_id_str, 0));
}

TEST_F(MediaStreamManagerTest, OnScreenCaptureOpened001) {
  std::string session_id_str = "session_id_str";

  ASSERT_NO_FATAL_FAILURE(media_stream_manager_->AsMediaStreamManagerExt()
                              ->OnScreenCaptureOpened(session_id_str));
}

TEST_F(MediaStreamManagerTest, RemoveNWebIdBySession001) {
  media::VideoCaptureSessionId session_id_1 = base::UnguessableToken::Create();

  ASSERT_NO_FATAL_FAILURE(
      media_stream_manager_->AsMediaStreamManagerExt()->RemoveNWebIdBySession(
          session_id_1));
}

TEST_F(MediaStreamManagerTest, RemoveNWebIdBySession002) {
  media::VideoCaptureSessionId session_id_1 = base::UnguessableToken::Create();
  media_stream_manager_->AsMediaStreamManagerExt()
      ->nweb_id_maps_[session_id_1.ToString()] = 0;
  ASSERT_NO_FATAL_FAILURE(
      media_stream_manager_->AsMediaStreamManagerExt()->RemoveNWebIdBySession(
          session_id_1));
}

TEST_F(MediaStreamManagerTest, AddNWebIdBySession001) {
  media::VideoCaptureSessionId session_id_1 = base::UnguessableToken::Create();
  int nweb_id = 1;
  ASSERT_NO_FATAL_FAILURE(
      media_stream_manager_->AsMediaStreamManagerExt()->AddNWebIdBySession(
          nweb_id, session_id_1));
}

TEST_F(MediaStreamManagerTest, AddNWebIdBySession002) {
  media::VideoCaptureSessionId session_id_1 = base::UnguessableToken::Create();
  int nweb_id = 1;
  media_stream_manager_->AsMediaStreamManagerExt()
      ->nweb_id_maps_[session_id_1.ToString()] = nweb_id;
  ASSERT_NO_FATAL_FAILURE(
      media_stream_manager_->AsMediaStreamManagerExt()->AddNWebIdBySession(
          nweb_id, session_id_1));
}

TEST_F(MediaStreamManagerTest, AddNWebIdBySession003) {
  media::VideoCaptureSessionId session_id_1 = base::UnguessableToken::Create();
  int nweb_id = 1;
  SessionIdState session_id_state;
  session_id_state.session_id = session_id_1.ToString();
  session_id_state.state = static_cast<ScreenCaptureState>(0);
  media_stream_manager_->AsMediaStreamManagerExt()->session_id_state_.push_back(session_id_state);
  ASSERT_NO_FATAL_FAILURE(
      media_stream_manager_->AsMediaStreamManagerExt()->AddNWebIdBySession(
          nweb_id, session_id_1));
}
#endif  // defined(ARKWEB_EX_SCREEN_CAPTURE)

#if BUILDFLAG(ARKWEB_WEBRTC)
TEST_F(MediaStreamManagerTest, GetNWebIdMatchStreamType001) {
  TestBrowserContext browser_context;
  std::unique_ptr<TestWebContents> web_contents = TestWebContents::Create(
      &browser_context, SiteInstanceImpl::Create(&browser_context));
  raw_ptr<RenderFrameHost> render_frame_host =
      web_contents->GetPrimaryMainFrame();
  GlobalRenderFrameHostId render_frame_host_id =
      render_frame_host->GetGlobalId();

  ASSERT_NO_FATAL_FAILURE(media_stream_manager_->AsMediaStreamManagerExt()
                              ->GetNWebIdMatchStreamType(render_frame_host_id));
}

TEST_F(MediaStreamManagerTest, GetNWebIdMatchStreamType002) {
  GlobalRenderFrameHostId render_frame_host_id {1, 1};

  ASSERT_NO_FATAL_FAILURE(media_stream_manager_->AsMediaStreamManagerExt()
                              ->GetNWebIdMatchStreamType(render_frame_host_id));
}
#endif