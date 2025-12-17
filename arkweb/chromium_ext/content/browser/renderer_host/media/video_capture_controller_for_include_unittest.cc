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

namespace content {
#if BUILDFLAG(ARKWEB_WEBRTC)
TEST_F(VideoCaptureControllerTest, PauseClientBySessionId001) {
  auto session_id_1 = base::UnguessableToken::Create();
  const VideoCaptureControllerID client_a_route_1 =
      base::UnguessableToken::Create();
  media::VideoCaptureParams session_params_1;
  session_params_1.requested_format = media::VideoCaptureFormat(
      gfx::Size(320, 240), 30, media::PIXEL_FORMAT_I420);
  controller_->AddClient(client_a_route_1, client_a_.get(), session_id_1,
                         session_params_1, std::nullopt);
  ASSERT_NO_FATAL_FAILURE(controller_->PauseClientBySessionId(session_id_1));
  ASSERT_NO_FATAL_FAILURE(controller_->PauseClientBySessionId(session_id_1));
  controller_->RemoveClient(client_a_route_1, client_a_.get()).is_empty();
  ASSERT_NO_FATAL_FAILURE(controller_->PauseClientBySessionId(session_id_1));
}

TEST_F(VideoCaptureControllerTest, ResumeClientBySessionId001) {
  auto session_id_1 = base::UnguessableToken::Create();
  const VideoCaptureControllerID client_a_route_1 =
      base::UnguessableToken::Create();
  media::VideoCaptureParams session_params_1;
  session_params_1.requested_format = media::VideoCaptureFormat(
      gfx::Size(320, 240), 30, media::PIXEL_FORMAT_I420);
  controller_->AddClient(client_a_route_1, client_a_.get(), session_id_1,
                         session_params_1, std::nullopt);
  ASSERT_NO_FATAL_FAILURE(controller_->ResumeClientBySessionId(session_id_1));
  controller_->PauseClientBySessionId(session_id_1);
  ASSERT_NO_FATAL_FAILURE(controller_->ResumeClientBySessionId(session_id_1));
  controller_->RemoveClient(client_a_route_1, client_a_.get()).is_empty();
  ASSERT_NO_FATAL_FAILURE(controller_->ResumeClientBySessionId(session_id_1));
}
#endif // ARKWEB_WEBRTC
} // namespace content