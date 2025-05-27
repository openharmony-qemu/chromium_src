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
 
#ifndef RENDER_WIDGET_HOST_IMPL_LOG_H_
#define RENDER_WIDGET_HOST_IMPL_LOG_H_
 
#define IGNORING_WEB_INPUT_FOR_EVENTS_RETURN(event) do {                             \
  if (agent_scheduling_group_->GetProcess()->IsBlocked()) {                          \
    LOG(INFO) << "IsIgnoringWebInputEvents for process blocked";                     \
    return true;                                                                     \
  }                                                                                  \
  if (!delegate_) {                                                                  \
    LOG(INFO) << "IsIgnoringWebInputEvents for delegate_ null";                      \
    return true;                                                                     \
  }                                                                                  \
  if (delegate_->ShouldIgnoreWebInputEvents(event)) {                                \
    LOG(INFO) << "IsIgnoringWebInputEvents for delegate ShouldIgnoreWebInputEvents"; \
    return true;                                                                     \
  }                                                                                  \
  return false;                                                                      \
} while(0)
 
#define IGNORING_WEB_INPUT_EVENTS_RETURN() do {                                      \
  if (agent_scheduling_group_->GetProcess()->IsBlocked()) {                          \
    LOG(INFO) << "IsIgnoringWebInputEvents for process blocked";                     \
    return true;                                                                     \
  }                                                                                  \
  if (!delegate_) {                                                                  \
    LOG(INFO) << "IsIgnoringWebInputEvents for delegate_ null";                      \
    return true;                                                                     \
  }                                                                                  \
  if (delegate_->ShouldIgnoreInputEvents()) {                                        \
    LOG(INFO) << "IsIgnoringWebInputEvents for delegate ShouldIgnoreWebInputEvents"; \
    return true;                                                                     \
  }                                                                                  \
  return false;                                                                      \
} while(0)
 
#endif