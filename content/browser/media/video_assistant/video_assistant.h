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

#ifndef CONTENT_BROWSER_MEDIA_VIDEO_ASSISTANT_VIDEO_ASSISTANT_H_
#define CONTENT_BROWSER_MEDIA_VIDEO_ASSISTANT_VIDEO_ASSISTANT_H_

#include <map>

#include "content/public/browser/media_player_id.h"
#include "media/mojo/mojom/media_player.mojom-forward.h"

namespace content {

class VideoAssistant {
 public:
  VideoAssistant();
  virtual ~VideoAssistant();

  virtual void EnableVideoAssistant(bool enable);
  virtual void EnableVideoAssistantAVCast(bool enable);
  virtual void ExecuteVideoAssistantFunction(const std::string& cmd_id);

  virtual bool Enabled();
  virtual bool AVCastEnabled();
  virtual void DidFinishNavigation();
  virtual void UpdateVideoAssistantConfig(
      const media::mojom::VideoAssistantConfigPtr& config);

  virtual void OnVideoPlaying(
      media::mojom::VideoAttributesForVASTPtr video_attributes,
      const MediaPlayerId& id);
  virtual void OnUpdateVideoAttributes(
      media::mojom::VideoAttributesForVASTPtr video_attributes,
      const MediaPlayerId& id);
  virtual void OnVideoDestroyed(const MediaPlayerId& id);

  virtual void ReportVideoDecoderName(const std::string& decoder_name);
};

} // namespace

#endif // CONTENT_BROWSER_MEDIA_VIDEO_ASSISTANT_VIDEO_ASSISTANT_H_
