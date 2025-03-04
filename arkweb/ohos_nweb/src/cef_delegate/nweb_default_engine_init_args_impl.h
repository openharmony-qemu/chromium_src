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

#ifndef WEB_DEFAULT_ENGINE_INIT_ARGS_IMPL_H
#define WEB_DEFAULT_ENGINE_INIT_ARGS_IMPL_H

#include "nweb.h"

namespace OHOS::NWeb {

class NWebDefaultEngineInitArgsImpl : public NWebEngineInitArgs {
 public:
  NWebDefaultEngineInitArgsImpl() = default;
  ~NWebDefaultEngineInitArgsImpl() = default;

  std::string GetDumpPath() override;
  bool GetIsFrameInfoDump() override;
  std::list<std::string> GetArgsToAdd() override;
  std::list<std::string> GetArgsToDelete() override;
  bool GetIsMultiRendererProcess() override;
  bool GetIsEnhanceSurface() override;
  bool GetIsPopup() override;

 private:
  std::string dump_path_ = "";
  bool frame_info_dump_ = false;
  std::list<std::string> web_engine_args_to_add_;
  std::list<std::string> web_engine_args_to_delete_;
  bool multi_renderer_process_ = false;
  bool is_enhance_surface_ = false;
  bool is_popup_ = false;
};

}  // namespace OHOS::NWeb

#endif  // WEB_ENGINE_INIT_ARGS_IMPL_H
