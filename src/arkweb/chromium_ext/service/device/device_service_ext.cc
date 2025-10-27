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

#if BUILDFLAG(ARKWEB_RENDER_REMOVE_BINDER)
void DeviceService::BindResSchedReport(
    mojo::PendingReceiver<mojom::ResSchedReport> receiver)
{
  if (!res_sched_report_) {
    res_sched_report_ = ResSchedReport::Create();
  }
  res_sched_report_->Bind(std::move(receiver));
}

void DeviceService::BindSysPropRenderObserver(
    mojo::PendingReceiver<mojom::SysPropRenderObserver> receiver)
{
  if (!sysprop_render_) {
    sysprop_render_ = SysPropRenderObserver::Create();
  }
  sysprop_render_->Bind(std::move(receiver));
}
#endif  // BUILDFLAG(ARKWEB_RENDER_REMOVE_BINDER)