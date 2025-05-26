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
  
#ifndef CC_INPUT_INPUT_HANDLER_UTILS_H_
#define CC_INPUT_INPUT_HANDLER_UTILS_H_

#include "cc/input/input_handler.h"

namespace cc {

class InputHandler;

class InputHandlerUtils {
public:
  InputHandlerUtils(InputHandler* handler);
  ~InputHandlerUtils();

#if BUILDFLAG(ARKWEB_INPUT_EVENTS)
  void HandleScrollUpdateForInternalBeginFrame(
      const viz::BeginFrameArgs& args);
#endif  // BUILDFLAG(ARKWEB_INPUT_EVENTS)

#if BUILDFLAG(ARKWEB_SAME_LAYER)
  LayerImpl* GetLayerImplIsHitByPoint(const gfx::Point& viewport_point);
  LayerImpl* GetNativeLayerImpl(const gfx::Point& viewport_point);
  LayerImpl* GetLayerImplById(int id);
  bool IsNativeLayer(gfx::PointF device_viewport_point);
#endif

private:
  raw_ptr<InputHandler> handler_;
};

}

#endif  //CC_INPUT_INPUT_HANDLER_UTILS_H_