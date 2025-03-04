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

#include "content/browser/media/video_assistant/video_assistant.h"

#include "media/mojo/mojom/media_player.mojom.h"

namespace content {

VideoAssistant::VideoAssistant() = default;
VideoAssistant::~VideoAssistant() = default;

void VideoAssistant::EnableVideoAssistant(bool enable) {}
void VideoAssistant::ExecuteVideoAssistantFunction(const std::string& cmd_id) {}

bool VideoAssistant::Enabled() {
  return false;
}
void VideoAssistant::DidFinishNavigation() {}
void VideoAssistant::UpdateVideoAssistantConfig(
    const media::mojom::VideoAssistantConfigPtr& config) {}

void VideoAssistant::OnVideoPlaying(
    media::mojom::VideoAttributesForVASTPtr video_attributes,
    const MediaPlayerId& id) {}
void VideoAssistant::OnUpdateVideoAttributes(
    media::mojom::VideoAttributesForVASTPtr video_attributes,
    const MediaPlayerId& id) {}
void VideoAssistant::OnVideoDestroyed(const MediaPlayerId& id) {}

}  // namespace content
