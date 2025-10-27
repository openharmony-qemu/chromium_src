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
#ifndef CEF_LIBCEF_BROWSER_ALLOY_RENDER_PROCESS_STATE_HANDLER_H_
#define CEF_LIBCEF_BROWSER_ALLOY_RENDER_PROCESS_STATE_HANDLER_H_

#include <vector>

struct WebComponentState {
  int nweb_id;
  bool state;
};

struct RenderProcessStateMap {
  uint32_t render_process_id;
  std::vector<WebComponentState> web_component_list;
};

class RenderProcessStateHandler {
 private:
  RenderProcessStateHandler();
  ~RenderProcessStateHandler();

  RenderProcessStateHandler(const RenderProcessStateHandler&) = delete;
  RenderProcessStateHandler& operator=(const RenderProcessStateHandler&) =
      delete;

  std::vector<RenderProcessStateMap> render_process_map_list_;

  std::vector<WebComponentState> initial_web_component_list_;

  static RenderProcessStateHandler* instance;

 public:
  static RenderProcessStateHandler* GetInstance();

  void UpdateRenderProcessState(uint32_t render_process_id,
                                int nweb_id,
                                bool is_to_background);

  void InitRenderProcessState(uint32_t render_process_id, int nweb_id);
};

#endif  // CEF_LIBCEF_BROWSER_ALLOY_RENDER_PROCESS_STATE_HANDLER_H_
