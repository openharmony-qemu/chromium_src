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

#include "nweb_default_engine_init_args_impl.h"

namespace OHOS::NWeb {

std::string NWebDefaultEngineInitArgsImpl::GetDumpPath() {
  return dump_path_;
}

bool NWebDefaultEngineInitArgsImpl::GetIsFrameInfoDump() {
  return frame_info_dump_;
}

std::list<std::string> NWebDefaultEngineInitArgsImpl::GetArgsToAdd() {
  return web_engine_args_to_add_;
}

std::list<std::string> NWebDefaultEngineInitArgsImpl::GetArgsToDelete() {
  return web_engine_args_to_delete_;
}

bool NWebDefaultEngineInitArgsImpl::GetIsMultiRendererProcess() {
  return multi_renderer_process_;
}

bool NWebDefaultEngineInitArgsImpl::GetIsEnhanceSurface() {
  return is_enhance_surface_;
}

bool NWebDefaultEngineInitArgsImpl::GetIsPopup() {
  return is_popup_;
}

}  // namespace OHOS::NWeb
