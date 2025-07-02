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

#ifndef ARKWEB_EXT_THIRD_PARTY_BLINK_RENDERER_MODULES_MEDIA_CONTROLS_ELEMENTS_MEDIA_CONTROL_PICTURE_IN_PICTURE_BUTTON_ELEMENT_UTILS_H_
#define ARKWEB_EXT_THIRD_PARTY_BLINK_RENDERER_MODULES_MEDIA_CONTROLS_ELEMENTS_MEDIA_CONTROL_PICTURE_IN_PICTURE_BUTTON_ELEMENT_UTILS_H_

#include "arkweb/build/features/features.h"

namespace blink {
class MediaControlPictureInPictureButtonElement;
class MediaControlsImpl;

class MediaControlPictureInPictureButtonElementUtils {
public:
    MediaControlPictureInPictureButtonElement* element;
    MediaControlPictureInPictureButtonElementUtils(MediaControlPictureInPictureButtonElement* element);

#if BUILDFLAG(ARKWEB_VIDEO_ASSISTANT)
    void SetSplitLineItemIsWanted(bool wanted);
#endif
};

}  // namespace blink

#endif  // ARKWEB_EXT_THIRD_PARTY_BLINK_RENDERER_MODULES_MEDIA_CONTROLS_ELEMENTS_MEDIA_CONTROL_PICTURE_IN_PICTURE_BUTTON_ELEMENT_UTILS_H_
