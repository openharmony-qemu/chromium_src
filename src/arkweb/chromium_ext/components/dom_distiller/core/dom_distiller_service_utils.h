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

#ifndef COMPONENTS_DOM_DISTILLER_CORE_DOM_DISTILLER_SERVICE_UTILS_H_
#define COMPONENTS_DOM_DISTILLER_CORE_DOM_DISTILLER_SERVICE_UTILS_H_

#include "components/dom_distiller/core/distiller_page.h"
#include "components/dom_distiller/core/dom_distiller_service.h"
#if BUILDFLAG(IS_ARKWEB_EXT)
#include "arkweb/ohos_nweb_ex/build/features/features.h"
#endif

namespace dom_distiller {

class DomDistillerService;

class DomDistillerServiceUtils {
 public:
  DomDistillerServiceUtils(DomDistillerService* service);

#if BUILDFLAG(ARKWEB_READER_MODE)
  bool ShouldViewUrl(const GURL& url,
                     const std::unique_ptr<DistillerPage>& distiller_page);
  bool ViewUrl(TaskTracker*& task_tracker,
               const std::unique_ptr<DistillerPage>& distiller_page);
  void AbortDistill();
#endif  // ARKWEB_READER_MODE
 private:
  raw_ptr<DomDistillerService> service_;
};
}  // namespace dom_distiller
#endif  // COMPONENTS_DOM_DISTILLER_CORE_DOM_DISTILLER_SERVICE_UTILS_H_