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

#ifndef CONTENT_BROWSER_MEDIA_SESSION_MEDIA_SESSION_CONTROLLERS_MANAGER_H_
#error "must be in include form CONTENT_BROWSER_MEDIA_SESSION_MEDIA_SESSION_CONTROLLERS_MANAGER_H_"
#endif

namespace content {

#if BUILDFLAG(ARKWEB_MEDIA_AVSESSION)
void MediaSessionControllersManager::OnEndAVSession(const MediaPlayerId& id,
                                                    bool is_hidden) {
  if (!IsMediaSessionEnabled()) {
    return;
  }
  auto controller = FindOrCreateController(id);
  if (controller) {
    controller->AsMediaSessionControllerExt()->OnEndAVSession(id, is_hidden);
  }
}
#endif // ARKWEB_MEDIA_AVSESSION

#if BUILDFLAG(ARKWEB_MEDIA_CAST)
void MediaSessionControllersManager::OnNotifyMeidaCastUri(const MediaPlayerId& id, const std::string& media_uri) {
  LOG(INFO) << "MediaSessionControllersManager::OnNotifyMeidaCastUri, mediaUri: " << media_uri;
  auto controller = FindOrCreateController(id);
  if (controller && controller->AsMediaSessionControllerExt()) {
    controller->AsMediaSessionControllerExt()->OnNotifyMeidaCastUri(media_uri);
  }
}

void MediaSessionControllersManager::CreateAVCastAdapter(const MediaPlayerId& id) {
  LOG(INFO) << "MediaSessionControllersManager::CreateAVCastAdapter";
  auto controller = FindOrCreateController(id);
  if (controller && controller->AsMediaSessionControllerExt()) {
    controller->AsMediaSessionControllerExt()->CreateAVCastAdapter();
  }
}

void MediaSessionControllersManager::HandleStopMediaCast(const MediaPlayerId& id) {
  LOG(INFO) << "MediaSessionControllersManager::HandleStopMediaCast";
  auto controller = FindOrCreateController(id);
  if (controller && controller->AsMediaSessionControllerExt()) {
    controller->AsMediaSessionControllerExt()->HandleStopMediaCast();
  }
}

void MediaSessionControllersManager::UpdateRemotePlayState(const MediaPlayerId& id, bool is_playing) {
  LOG(INFO) << "MediaSessionControllersManager::UpdateRemotePlayState";
  auto controller = FindOrCreateController(id);
  if (controller && controller->AsMediaSessionControllerExt()) {
    controller->AsMediaSessionControllerExt()->UpdateRemotePlayState(is_playing);
  }
}

void MediaSessionControllersManager::UpdateRemotePlayPosition(const MediaPlayerId& id, int64_t position) {
  LOG(INFO) << "MediaSessionControllersManager::UpdateRemotePlayPosition";
  auto controller = FindOrCreateController(id);
  if (controller && controller->AsMediaSessionControllerExt()) {
    controller->AsMediaSessionControllerExt()->UpdateRemotePlayPosition(position);
  }
}

void MediaSessionControllersManager::MediaCastStopByNavigation() {
  if (!IsMediaSessionEnabled())
    return;
  for (auto& entry : controllers_map_) {
    if (entry.second->AsMediaSessionControllerExt()) {
      entry.second->AsMediaSessionControllerExt()->MediaCastStopByNavigation();
    }
  }
}

void MediaSessionControllersManager::SetPauseByAvcast(const MediaPlayerId& id, bool pause_avcast) {
  LOG(INFO) << "MediaSessionControllersManager::SetPauseByAvcast";
  auto controller = FindOrCreateController(id);
  if (controller && controller->AsMediaSessionControllerExt()) {
    controller->AsMediaSessionControllerExt()->SetPauseByAvcast(pause_avcast);
  }
}
#endif // BUILDFLAG(ARKWEB_MEDIA_CAST)

}