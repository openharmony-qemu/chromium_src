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

#ifndef COMPONENTS_DOM_DISTILLER_CORE_DISTILLER_OHOS_H_
#define COMPONENTS_DOM_DISTILLER_CORE_DISTILLER_OHOS_H_

#include <string>

#include "base/memory/raw_ptr.h"
#include "base/memory/weak_ptr.h"
#include "base/timer/timer.h"
#include "components/dom_distiller/core/article_distillation_update.h"
#include "components/dom_distiller/core/distiller_page.h"
#include "components/dom_distiller/core/distiller_url_fetcher.h"
#include "components/dom_distiller/core/proto/distilled_article.pb.h"
#include "url/gurl.h"
#if BUILDFLAG(IS_ARKWEB_EXT)
#include "arkweb/ohos_nweb_ex/build/features/features.h"
#endif

namespace dom_distiller {
class DistillerImpl;
class DistillerOhos {
 public:
#if BUILDFLAG(ARKWEB_READER_MODE)
  virtual void AbortDistill() = 0;
#endif  // ARKWEB_READER_MODE
};

class DistillerImplUtils {
 public:
  DistillerImplUtils(DistillerImpl* impl);
#if BUILDFLAG(ARKWEB_READER_MODE)
  void DistillPage();
  void DistillNextPage(int page_num, const GURL& url);
  void OnHwPageDistillationFinished(
      int page_num,
      const GURL& page_url,
      std::unique_ptr<proto::DomDistillerResult> distilled_page,
      bool distillation_successful);
#endif  // ARKWEB_READER_MODE
 private:
#if BUILDFLAG(ARKWEB_READER_MODE)
  void OnHwReadExtendFields(const proto::DomDistillerResult& result,
                            DistilledPageProto_HwReadExtendFields* fields,
                            const GURL& page_url,
                            bool is_distill_catalog);
  void WriteMutableFlatFields(const proto::DomDistillerResult& distiller_result, int page_num);
  void WriteFlatFields(const proto::DomDistillerResult& distiller_result, int page_num, const GURL& page_url);
  void AddToDistillationQueueForContentInfo(
      const proto::DomDistillerResult& distiller_result,
      const GURL& page_url,
      int curr_page);
  void AddToDistillationQueueForCatalogInfo(
      const proto::DomDistillerResult& distiller_result,
      const GURL& page_url,
      int curr_page);
  void ReadContentInfo(const proto::HwReadExtendFields& result,
                       DistilledPageProto_HwReadExtendFields* fields,
                       bool is_distill_catalog);
  void ReadPaginationInfo(const proto::HwReadExtendFields& result,
                          DistilledPageProto_HwReadExtendFields_CatalogPageInfo* fields);
  void ReadCatalogInfo(const proto::HwReadExtendFields& result,
                       DistilledPageProto_HwReadExtendFields* fields,
                       bool is_distill_catalog);
  base::TimeTicks start_distill_time_;
#endif  // ARKWEB_READER_MODE
  raw_ptr<DistillerImpl> impl_;
  base::WeakPtrFactory<DistillerImplUtils> weak_factory_{this};
};
}  // namespace dom_distiller
#endif  // COMPONENTS_DOM_DISTILLER_CORE_DISTILLER_OHOS_H_