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
#include "cef/ohos_cef_ext/libcef/browser/alloy/render_process_state_handler.h"

#include "base/logging.h"
#include "base/trace_event/trace_event.h"
#include "third_party/ohos_ndk/includes/ohos_adapter/res_sched_client_adapter.h"

RenderProcessStateHandler* RenderProcessStateHandler::instance = nullptr;

RenderProcessStateHandler::RenderProcessStateHandler() {}

RenderProcessStateHandler* RenderProcessStateHandler::GetInstance() {
  if (instance == nullptr) {
    instance = new RenderProcessStateHandler();
  }
  return instance;
}

RenderProcessStateHandler::~RenderProcessStateHandler() {
  instance = nullptr;
}

void RenderProcessStateHandler::UpdateRenderProcessState(
    uint32_t render_process_id,
    int nweb_id,
    bool is_to_background) {
  using namespace OHOS::NWeb;
  if (render_process_id == 0) {
    LOG(DEBUG) << "RenderProcessStateHandler::UpdateRenderProcessState: render "
                  "process has not init, nweb_id: "
               << nweb_id << " is_to_background: " << is_to_background;
    for (WebComponentState& web_item : initial_web_component_list_) {
      if (web_item.nweb_id == nweb_id) {
        web_item.state = is_to_background;
        return;
      }
    }

    WebComponentState initial_web_component = {nweb_id, is_to_background};
    initial_web_component_list_.push_back(initial_web_component);
    return;
  }

  LOG(DEBUG)
      << "RenderProcessStateHandler::UpdateRenderProcessState: render_id:"
      << render_process_id << " nweb_id: " << nweb_id
      << " is_to_background: " << is_to_background;
  bool report_background_state = is_to_background;
  bool is_render_init = true;
  for (RenderProcessStateMap& item : render_process_map_list_) {
    LOG(DEBUG)
        << "RenderProcessStateHandler::UpdateRenderProcessState: for_render_id:"
        << item.render_process_id;
    if (item.render_process_id != render_process_id) {
      continue;
    }
    is_render_init = false;

    bool is_web_init = true;
    for (WebComponentState& web_component_state : item.web_component_list) {
      LOG(DEBUG) << "RenderProcessStateHandler::UpdateRenderProcessState: "
                    "item.nweb_id: "
                 << web_component_state.nweb_id
                 << " state: " << web_component_state.state;
      if (web_component_state.nweb_id != nweb_id) {
        if (report_background_state && !web_component_state.state) {
          report_background_state = false;
        }
        continue;
      }
      is_web_init = false;
      web_component_state.state = is_to_background;
    }

    if (is_web_init) {
      LOG(DEBUG) << "RenderProcessStateHandler::UpdateRenderProcessState: add "
                    "new web: "
                 << nweb_id << " state: " << is_to_background;

      WebComponentState new_web_component = {nweb_id, is_to_background};
      item.web_component_list.push_back(new_web_component);
    }
  }

  if (is_render_init) {
    LOG(DEBUG) << "RenderProcessStateHandler::UpdateRenderProcessState: add "
                  "new render: "
               << render_process_id << " nweb_id: " << nweb_id
               << " state: " << is_to_background;
    WebComponentState new_web_component = {nweb_id, is_to_background};
    std::vector<WebComponentState> web_component_list;
    web_component_list.push_back(new_web_component);
    RenderProcessStateMap new_render_process_state_map = {render_process_id,
                                                          web_component_list};
    render_process_map_list_.push_back(new_render_process_state_map);
  }

  LOG(INFO) << "RenderProcessStateHandler::UpdateRenderProcessState: "
               "ReportRenderProcessStatus render_id: "
            << render_process_id
            << " report_background_state: " << report_background_state;
  ResSchedStatusAdapter status = report_background_state
                                     ? ResSchedStatusAdapter::WEB_INACTIVE
                                     : ResSchedStatusAdapter::WEB_ACTIVE;
  ResSchedClientAdapter::ReportRenderProcessStatus(status, render_process_id);
  TRACE_EVENT2("base", "ResSchedClientAdapter::ReportRenderProcessStatus",
               "render_process_id", render_process_id, "status",
               static_cast<int32_t>(status));

  for (auto item = initial_web_component_list_.begin();
       item != initial_web_component_list_.end();) {
    if (item->nweb_id == nweb_id) {
      LOG(DEBUG) << "UpdateRenderProcessState: delete from init list: "
                 << " nweb_id: " << nweb_id << " state: " << item->state;
      item = initial_web_component_list_.erase(item);
    } else {
      ++item;
    }
  }
}

void RenderProcessStateHandler::InitRenderProcessState(
    uint32_t render_process_id,
    int nweb_id) {
  LOG(DEBUG) << "RenderProcessStateHandler::InitRenderProcessState: render_id: "
             << render_process_id << " nweb_id: " << nweb_id;
  if (initial_web_component_list_.size() > 0) {
    for (WebComponentState& web_component_state : initial_web_component_list_) {
      if (web_component_state.nweb_id == nweb_id) {
        UpdateRenderProcessState(render_process_id, nweb_id, web_component_state.state);
        LOG(DEBUG) << "RenderProcessStateHandler::InitRenderProcessState: size: "
                   << initial_web_component_list_.size();
        return;
      }
    }
    return;
  }
  // browser click search. same web component, render has been changed.
  for (RenderProcessStateMap& item : render_process_map_list_) {
    for (WebComponentState& web_component_state : item.web_component_list) {
      if (web_component_state.nweb_id == nweb_id) {
        LOG(DEBUG) << "RenderProcessStateHandler::InitRenderProcessState: render_id has been changed: "
                   << item.render_process_id << " -> " << render_process_id
                   << " nweb_id: " << web_component_state.nweb_id
                   << " state: " << web_component_state.state;
        // report last web componet state.
        UpdateRenderProcessState(render_process_id, nweb_id, web_component_state.state);
        return;
      }
    }
  }
}
 