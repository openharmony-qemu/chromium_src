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
 
#ifndef SPARE_RENDER_PROCESS_MANAGER_H_
#define SPARE_RENDER_PROCESS_MANAGER_H_
 
#include <cstdint>
#include <memory>
#include <mutex>
 
#include "base/memory/raw_ptr.h"
#include "base/time/time.h"
#include "base/timer/timer.h"
#include "content/public/browser/spare_render_process_host_manager.h"
#include "base/no_destructor.h"
 
namespace content {
class BrowserContext;
}  // namespace content
 
namespace OHOS::NWeb {
 
static constexpr int32_t WARMUP_SPARE_RENDER_REPORT_NWEB_ID = 0;
static constexpr base::TimeDelta kSpareRenderTimeout = base::Seconds(2);
 
class SpareRenderProcessManagerOhos
    : public content::SpareRenderProcessHostManager::Observer {
 public:
   friend class base::NoDestructor<SpareRenderProcessManagerOhos>;
 
  enum class State {
    IDLE = 0,     // No spare process
    NOT_FORK = 1,  // triggered but not forked
    ACTIVE = 2,   // Spare exists, timeout running (fork→taken/timeout)
    INACTIVE = 3,  // Timed out, priority lowered
    RELEASED = 4     // Released WARMUP_SPARE_RENDER_REPORT_NWEB_ID
  };
 
  static SpareRenderProcessManagerOhos& GetInstance();
  void WarmupSpare(content::BrowserContext* browser_context);
 
  // Called from LaunchChildProcess after fork succeeds.
  // Records PID and starts timeout timer.
  void NotifyWarmupSpareLaunched(pid_t pid);
 
  // Called when MaybeTakeSpare successfully assigns the spare.
  void OnSpareTaken();
 
  State GetState() const;
  bool IsSpareRenderProcess(pid_t pid) const;
  pid_t GetSpareRenderPid() const;
  void SetWarmupWindowId(int32_t window_id);
  int32_t GetWarmupWindowId() const;
 
  static void SetWarmupEnabled(bool enabled);
  static bool IsWarmupEnabled();
 
  // SpareRenderProcessHostManager::Observer implementation.
  void OnSpareRenderProcessHostReady(
      content::RenderProcessHost* host) override;
  void OnSpareRenderProcessHostRemoved(
      content::RenderProcessHost* host) override;
 
 private:
  SpareRenderProcessManagerOhos();
  ~SpareRenderProcessManagerOhos() override;
 
  void UpdateRenderState(State new_state);
  void OnRenderActiveTimeout();
  void ReportSpareInactive(pid_t pid);
  void UnregisterSpareNWebId();
  void RegisterObserver();
  void UnregisterObserver();
 
  State state_ = State::IDLE;
  pid_t spare_render_pid_ = -1;
  int32_t window_id_ = -1;
  raw_ptr<content::RenderProcessHost> spare_rph_ = nullptr;
 
  std::unique_ptr<base::OneShotTimer> render_active_timer_;
  mutable std::mutex mutex_;
  static bool enabled_;
  bool spare_render_taken_ = false;
};
 
}  // namespace OHOS::NWeb
 
#endif  // SPARE_RENDER_PROCESS_MANAGER_H_