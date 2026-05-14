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
 
#include "spare_render_process_manager_ohos.h"
#include "content/browser/renderer_host/render_process_host_impl.h"
#include "content/public/browser/browser_context.h"
#include "content/public/browser/browser_thread.h"
#include "content/public/browser/render_process_host.h"
#include "third_party/ohos_ndk/includes/ohos_adapter/ohos_adapter_helper.h"
#include "third_party/ohos_ndk/includes/ohos_adapter/res_sched_client_adapter.h"
 
namespace OHOS::NWeb {
 
bool SpareRenderProcessManagerOhos::enabled_ = false;
 
SpareRenderProcessManagerOhos& SpareRenderProcessManagerOhos::GetInstance() {
  static base::NoDestructor<SpareRenderProcessManagerOhos> instance;
  return *instance;
}
 
SpareRenderProcessManagerOhos::SpareRenderProcessManagerOhos() = default;
 
SpareRenderProcessManagerOhos::~SpareRenderProcessManagerOhos() {
  UnregisterObserver();
}
 
void SpareRenderProcessManagerOhos::SetWarmupEnabled(bool enabled) {
  enabled_ = enabled;
  LOG(INFO) << "SpareRenderProcessManagerOhos::SetEnabled: " << enabled;
}
 
bool SpareRenderProcessManagerOhos::IsWarmupEnabled() {
  return enabled_;
}
 
int32_t SpareRenderProcessManagerOhos::GetWarmupWindowId() const {
  std::lock_guard<std::mutex> lock(mutex_);
  return window_id_;
}
 
void SpareRenderProcessManagerOhos::SetWarmupWindowId(int32_t window_id) {
  std::lock_guard<std::mutex> lock(mutex_);
  window_id_ = window_id;
  LOG(INFO) << "SpareRenderProcessManagerOhos::SetWarmupWindowId, window_id=" << window_id;
}
 
void SpareRenderProcessManagerOhos::WarmupSpare(
    content::BrowserContext* browser_context) {
  if (!content::BrowserThread::CurrentlyOn(content::BrowserThread::UI)) {
    return;
  }
  std::lock_guard<std::mutex> lock(mutex_);
  if (state_ != State::IDLE) {
    return;
  }
  if (!browser_context || browser_context->ShutdownStarted()) {
    return;
  }
  if (content::RenderProcessHostImpl::GetProcessCountForLimit() >=
      content::RenderProcessHost::GetMaxRendererProcessCount()) {
    return;
  }
  UpdateRenderState(State::NOT_FORK);
 
  RegisterObserver();
 
  content::SpareRenderProcessHostManager::Get().WarmupSpare(browser_context);
  LOG(INFO) << "SpareRenderProcessManagerOhos triggered";
}
 
void SpareRenderProcessManagerOhos::NotifyWarmupSpareLaunched(pid_t pid) {
  std::lock_guard<std::mutex> lock(mutex_);
  if (state_ != State::NOT_FORK) {
    return;
  }
  spare_render_pid_ = pid;
  UpdateRenderState(State::ACTIVE);
  
  // Start timeout timer right after fork.
  render_active_timer_ = std::make_unique<base::OneShotTimer>();
  if (!spare_render_taken_) {
    render_active_timer_->Start(
        FROM_HERE, kSpareRenderTimeout,
        base::BindOnce(&SpareRenderProcessManagerOhos::OnRenderActiveTimeout,
                      base::Unretained(this)));
    LOG(INFO) << "SpareRenderProcessManagerOhos: render process launched, pid=" << pid
            << ", timer started";
  }
}
 
// Observer: mojo channel connected.
void SpareRenderProcessManagerOhos::OnSpareRenderProcessHostReady(
    content::RenderProcessHost* host) {
  std::lock_guard<std::mutex> lock(mutex_);
  if (state_ != State::ACTIVE && state_ != State::INACTIVE) {
    return;
  }
  spare_rph_ = host;
  // PID already set by NotifyWarmupSpareLaunched.
  LOG(INFO) << "Spare render process mojo ready, pid=" << spare_render_pid_;
}
 
// Observer: spare removed from management.
void SpareRenderProcessManagerOhos::OnSpareRenderProcessHostRemoved(
    content::RenderProcessHost* host) {
  std::lock_guard<std::mutex> lock(mutex_);
  if (state_ == State::IDLE || state_ == State::NOT_FORK|| state_ == State::RELEASED) {
    return;
  }
 
  if (render_active_timer_) {
    render_active_timer_->Stop();
  }
 
  if (state_ == State::ACTIVE) {
    ReportSpareInactive(spare_render_pid_);
    UnregisterSpareNWebId();
  } else if (state_ == State::INACTIVE) {
    // INACTIVE already reported in OnTimeout, just clean up.
    UnregisterSpareNWebId();
  }
 
  UpdateRenderState(State::RELEASED);
  UnregisterObserver();
}
 
// Called when MaybeTakeSpare succeeds.
void SpareRenderProcessManagerOhos::OnSpareTaken() {
  std::lock_guard<std::mutex> lock(mutex_);
  
  spare_render_taken_ = true;
  LOG(INFO) << "spare render process taken by navigation, spare_render_pid=" << spare_render_pid_;
  if (state_ != State::ACTIVE && state_ != State::INACTIVE) {
    return;
  }
 
  if (render_active_timer_) {
    render_active_timer_->Stop();
  }
 
  UnregisterSpareNWebId();
  UpdateRenderState(State::RELEASED);
  UnregisterObserver();
}
 
void SpareRenderProcessManagerOhos::OnRenderActiveTimeout() {
  std::lock_guard<std::mutex> lock(mutex_);
  if (state_ != State::ACTIVE) {
    return;
  }
  LOG(INFO) << "warmup spare render process active status timeout, demoting to INACTIVE, pid="
            << spare_render_pid_;
  ReportSpareInactive(spare_render_pid_);
  UnregisterSpareNWebId();
  UpdateRenderState(State::INACTIVE);
}
 
SpareRenderProcessManagerOhos::State SpareRenderProcessManagerOhos::GetState() const {
  std::lock_guard<std::mutex> lock(mutex_);
  return state_;
}
 
bool SpareRenderProcessManagerOhos::IsSpareRenderProcess(pid_t pid) const {
  std::lock_guard<std::mutex> lock(mutex_);
  return spare_render_pid_ == pid && pid > 0 &&
         (state_ == State::ACTIVE || state_ == State::INACTIVE);
}
 
pid_t SpareRenderProcessManagerOhos::GetSpareRenderPid() const {
  std::lock_guard<std::mutex> lock(mutex_);
  return spare_render_pid_;
}
 
void SpareRenderProcessManagerOhos::ReportSpareInactive(pid_t pid) {
  OHOS::NWeb::ResSchedClientAdapter::ReportWindowStatus(
      OHOS::NWeb::ResSchedStatusAdapter::WEB_INACTIVE,
      pid, window_id_, WARMUP_SPARE_RENDER_REPORT_NWEB_ID);
}
 
void SpareRenderProcessManagerOhos::UnregisterSpareNWebId() {
  OHOS::NWeb::ResSchedClientAdapter::ReportNWebInit(
      OHOS::NWeb::ResSchedStatusAdapter::WEB_SCENE_EXIT, WARMUP_SPARE_RENDER_REPORT_NWEB_ID);
}
 
void SpareRenderProcessManagerOhos::RegisterObserver() {
  content::SpareRenderProcessHostManager::Get().AddObserver(this);
}
 
void SpareRenderProcessManagerOhos::UnregisterObserver() {
  content::SpareRenderProcessHostManager::Get().RemoveObserver(this);
}
 
void SpareRenderProcessManagerOhos::UpdateRenderState(State new_state) {
  LOG(INFO) << "SpareRenderProcessManagerOhos: "
            << static_cast<int>(state_) << " -> " << static_cast<int>(new_state);
  state_ = new_state;
}
 
}  // namespace OHOS::NWeb