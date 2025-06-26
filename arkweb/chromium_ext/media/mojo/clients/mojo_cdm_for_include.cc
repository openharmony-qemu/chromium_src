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

#ifndef MEDIA_MOJO_CLIENTS_MOJO_CDM_H_
#error "must be in include form MEDIA_MOJO_CLIENTS_MOJO_CDM_H_"
#endif

namespace media {
#if BUILDFLAG(ARKWEB_ENABLE_WISEPLAY)
void MojoCdm::SuspendCdmSession() {
  LOG(INFO) << "[DRM]" << __func__;
  if (!remote_cdm_) {
    return;
  }
  remote_cdm_->SuspendSession();
}

void MojoCdm::ResumeCdmSession() {
  LOG(INFO) << "[DRM]" << __func__;
  if (!remote_cdm_) {
    return;
  }
  remote_cdm_->ResumeSession();
}
#endif
}