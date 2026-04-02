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

#include "arkweb/chromium_ext/components/dom_distiller/core/distiller_ohos.h"

#include "base/functional/bind.h"
#include "base/functional/callback.h"
#include "build/build_config.h"
#include "components/dom_distiller/core/distiller.h"
#include "components/dom_distiller/core/proto/distilled_article.pb.h"
#include "components/dom_distiller/core/proto/distilled_page.pb.h"
#include "base/logging.h"

namespace dom_distiller {

DistillerImplUtils::DistillerImplUtils(DistillerImpl* impl) : impl_(impl) {}

#if BUILDFLAG(ARKWEB_READER_MODE)
void DistillerImplUtils::DistillPage() {
  if (const auto* options = impl_->distiller_page_->GetDistillOptions()) {
    impl_->SetMaxNumPagesInArticle(options->max_distill_pages);
  }
}

void DistillerImplUtils::DistillNextPage(int page_num, const GURL& url) {
  LOG(INFO) << __func__ << "[Distiller] page_num: " << page_num;
  start_distill_time_ = base::TimeTicks::Now();
  impl_->distiller_page_->DistillPage(
      url, impl_->dom_distiller_options_,
      base::BindRepeating(&DistillerImplUtils::OnHwPageDistillationFinished,
                          weak_factory_.GetWeakPtr(), page_num, url));
}

void DistillerImplUtils::ReadContentInfo(const proto::HwReadExtendFields& result,
  DistilledPageProto_HwReadExtendFields* fields, bool is_distill_catalog) {
  if (is_distill_catalog || !result.has_content_info()) {
    return;
  }
  auto* content_info = fields->mutable_content_info();
  GURL next_page_url(result.content_info().next_page());
  if (next_page_url.is_valid()) {
    content_info->set_next_page(next_page_url.spec());
  }

  GURL prev_page_url(result.content_info().prev_page());
  if (prev_page_url.is_valid()) {
    content_info->set_prev_page(prev_page_url.spec());
  }

  GURL catalog_page_url(result.content_info().catalog_page());
  if (catalog_page_url.is_valid()) {
    content_info->set_catalog_page(catalog_page_url.spec());
  }
}

void DistillerImplUtils::ReadPaginationInfo(const proto::HwReadExtendFields& result,
  DistilledPageProto_HwReadExtendFields_CatalogPageInfo* catalog_info) {
  if (!result.catalog_info().has_pagination_info()) {
    return;
  }
  auto* pagination_info = catalog_info->mutable_pagination_info();
  GURL next_catalog_page_url(
      result.catalog_info().pagination_info().next_catalog_page());
  if (next_catalog_page_url.is_valid()) {
    pagination_info->set_next_catalog_page(next_catalog_page_url.spec());
  }

  GURL prev_catalog_page_url(
      result.catalog_info().pagination_info().prev_catalog_page());
  if (prev_catalog_page_url.is_valid()) {
    pagination_info->set_prev_catalog_page(prev_catalog_page_url.spec());
  }

  GURL whole_catalog_page_url(
      result.catalog_info().pagination_info().whole_catalog_page());
  if (whole_catalog_page_url.is_valid()) {
    pagination_info->set_whole_catalog_page(whole_catalog_page_url.spec());
  }
  DistilledPageProto_HwReadExtendFields_CatalogPageInfo_CatalogPaginationInfo_CatalogPageContent
      all_catalog_pages;
  for (int i = 0;
        i < result.catalog_info().pagination_info().all_catalog_pages_size();
        i++) {
    GURL pageurl(result.catalog_info()
                      .pagination_info()
                      .all_catalog_pages(i)
                      .page_url());
    if (pageurl.is_valid()) {
      all_catalog_pages.set_page_name(result.catalog_info()
                                          .pagination_info()
                                          .all_catalog_pages(i)
                                          .page_name());
      all_catalog_pages.set_page_url(pageurl.spec());
      all_catalog_pages.set_selected(result.catalog_info()
                                          .pagination_info()
                                          .all_catalog_pages(i)
                                          .selected());
      *pagination_info->add_all_catalog_pages() = all_catalog_pages;
    }
  }
}

void DistillerImplUtils::ReadCatalogInfo(const proto::HwReadExtendFields& result,
  DistilledPageProto_HwReadExtendFields* fields, bool is_distill_catalog) {
  if (!is_distill_catalog || !result.has_catalog_info()) {
    return;
  }

  auto* catalog_info = fields->mutable_catalog_info();
  if (!result.catalog_info().has_chapter_list()) {
    return;
  }
  auto& chapter_list = result.catalog_info().chapter_list();
  DistilledPageProto_HwReadExtendFields_CatalogPageInfo_ChapterInfo
      latest_chapters;
  for (int i = 0; i < chapter_list.latest_chapters_size(); i++) {
    latest_chapters.set_chapter_name(
        chapter_list.latest_chapters(i).chapter_name());
    latest_chapters.set_chapter_url(
        chapter_list.latest_chapters(i).chapter_url());
    *catalog_info->add_latest_chapters() = latest_chapters;
  }
  DistilledPageProto_HwReadExtendFields_CatalogPageInfo_ChapterInfo
      all_chapters;
  for (int i = 0; i < chapter_list.all_chapters_size(); i++) {
    all_chapters.set_chapter_name(
        chapter_list.all_chapters(i).chapter_name());
    all_chapters.set_chapter_url(
        chapter_list.all_chapters(i).chapter_url());
    *catalog_info->add_all_chapters() = all_chapters;
  }

  ReadPaginationInfo(result, catalog_info);
}

void DistillerImplUtils::OnHwReadExtendFields(const proto::DomDistillerResult& distiller_result,
  DistilledPageProto_HwReadExtendFields* fields, const GURL& page_url, bool is_distill_catalog) {
  const auto& result = distiller_result.flat_fields();
  if (result.has_book_name()) {
    fields->set_book_name(result.book_name());
  }
  if (result.has_chapter_name()) {
    fields->set_chapter_name(result.chapter_name());
  }
  if (result.has_description()) {
    fields->set_description(result.description());
  }
  if (result.has_img_url()) {
    GURL img_url(result.img_url());
    if (img_url.is_valid()) {
      fields->set_img_url(img_url.spec());
    }
  }
  if (result.has_author()) {
    fields->set_author(result.author());
  }
  if (result.has_content()) {
    fields->set_content(result.content());
  }
  if (result.has_category()) {
    fields->set_category(result.category());
  }
  if (result.has_last_chapter_name()) {
    fields->set_last_chapter_name(result.last_chapter_name());
  }
  if (result.has_last_chapter_url()) {
    GURL last_chapter_url(result.last_chapter_url());
    if (last_chapter_url.is_valid()) {
      fields->set_last_chapter_url(last_chapter_url.spec());
    }
  }
  if (result.has_start_read_link()) {
    GURL start_read_link(result.start_read_link());
    if (start_read_link.is_valid()) {
      fields->set_start_read_link(start_read_link.spec());
    }
  }
  if (result.has_finished_status()) {
    fields->set_finished_status(result.finished_status());
  }
  ReadContentInfo(result, fields, is_distill_catalog);
  ReadCatalogInfo(result, fields, is_distill_catalog);
}

void DistillerImplUtils::AddToDistillationQueueForContentInfo(
    const proto::DomDistillerResult& distiller_result,
    const GURL& page_url,
    int curr_page) {
  FetchAction fetch_action = FetchAction::BACKWARD;
  if (const auto* options = impl_->distiller_page_->GetDistillOptions()) {
    fetch_action = options->fetch_action;
  }

  GURL fetch_page_url;
  if (fetch_action == FetchAction::BACKWARD &&
      distiller_result.flat_fields().content_info().has_next_page()) {
    fetch_page_url =
        GURL(distiller_result.flat_fields().content_info().next_page());
  } else if (fetch_action == FetchAction::FORWARD &&
             distiller_result.flat_fields().content_info().has_prev_page()) {
    fetch_page_url =
        GURL(distiller_result.flat_fields().content_info().prev_page());
  }

  if (!fetch_page_url.is_empty() && fetch_page_url.is_valid()) {
    impl_->AddToDistillationQueue(curr_page + 1, fetch_page_url);
  }
}

void DistillerImplUtils::AddToDistillationQueueForCatalogInfo(
    const proto::DomDistillerResult& distiller_result,
    const GURL& page_url,
    int curr_page) {
  if (!distiller_result.flat_fields().catalog_info().has_pagination_info()) {
    return;
  }

  FetchAction fetch_action = FetchAction::BACKWARD;
  if (const auto* options = impl_->distiller_page_->GetDistillOptions()) {
    fetch_action = options->fetch_action;
  }

  GURL fetch_page_url;
  const auto& pagination_info =
      distiller_result.flat_fields().catalog_info().pagination_info();

  if (fetch_action == FetchAction::BACKWARD) {
    if (pagination_info.has_next_catalog_page()) {
      fetch_page_url = GURL(pagination_info.next_catalog_page());
    } else if (pagination_info.has_whole_catalog_page()) {
      fetch_page_url = GURL(pagination_info.whole_catalog_page());
    }
  } else if (fetch_action == FetchAction::FORWARD) {
    if (pagination_info.has_prev_catalog_page()) {
      fetch_page_url = GURL(pagination_info.prev_catalog_page());
    }
  }

  if (!fetch_page_url.is_empty() && fetch_page_url.is_valid()) {
    impl_->AddToDistillationQueue(curr_page + 1, fetch_page_url);
  }
}

void DistillerImplUtils::WriteMutableFlatFields(const proto::DomDistillerResult& distiller_result,
  int page_num) {
  DistillerImpl::DistilledPageData* page_data =
    impl_->GetPageAtIndex(impl_->started_pages_index_[page_num]);
  auto* fields = page_data->distilled_page_proto->data.mutable_flat_fields();
  if (distiller_result.has_distilled_result()) {
    if (distiller_result.distilled_result().has_code()) {
      auto* distilled_result = fields->mutable_distilled_result();
      distilled_result->set_code(distiller_result.distilled_result().code());
    }
    if (distiller_result.distilled_result().has_message()) {
      auto* distilled_result = fields->mutable_distilled_result();
      distilled_result->set_message(
          distiller_result.distilled_result().message());
    }
  }
  auto* timing_info = fields->mutable_timing_info();
  if (distiller_result.has_timing_info()) {
    if (distiller_result.timing_info().has_total_time()) {
      timing_info->set_distill_time(
          distiller_result.timing_info().total_time());
    }
  }
  float total_time =
      (base::TimeTicks::Now() - start_distill_time_).InMillisecondsF();
  timing_info->set_total_time(total_time);
}

void DistillerImplUtils::WriteFlatFields(const proto::DomDistillerResult& distiller_result, int page_num,
  const GURL& page_url) {
  if (!distiller_result.has_flat_fields()) {
    LOG(ERROR) << __func__ << "[Distiller] no flat fields";
    return;
  }
  bool is_distill_catalog = false;
  if (const auto* distill_options = impl_->distiller_page_->GetDistillOptions()) {
    is_distill_catalog = distill_options->distill_type == DistillType::NOVEL_BOOKDETAIL;
  }
  DistillerImpl::DistilledPageData* page_data =
      impl_->GetPageAtIndex(impl_->started_pages_index_[page_num]);
  OnHwReadExtendFields(
      distiller_result,
      page_data->distilled_page_proto->data.mutable_flat_fields(), page_url,
      is_distill_catalog);

  if (!is_distill_catalog &&
      distiller_result.flat_fields().has_content_info()) {
    AddToDistillationQueueForContentInfo(distiller_result, page_url,
                                         page_num);
  } else if (is_distill_catalog &&
              distiller_result.flat_fields().has_catalog_info()) {
    AddToDistillationQueueForCatalogInfo(distiller_result, page_url,
                                         page_num);
  }
}

void DistillerImplUtils::OnHwPageDistillationFinished(
    int page_num,
    const GURL& page_url,
    std::unique_ptr<proto::DomDistillerResult> distiller_result,
    bool distillation_successful) {
  CHECK(impl_->started_pages_index_.find(page_num) != impl_->started_pages_index_.end());
  if (!distillation_successful) {
    impl_->started_pages_index_.erase(page_num);
    impl_->RunDistillerCallbackIfDone();
    return;
  }

  CHECK(distiller_result);
  CHECK_LT(impl_->started_pages_index_[page_num], impl_->pages_.size())
      << "started_pages_index_[" << page_num
      << "] (=" << impl_->started_pages_index_[page_num]
      << ") is out of range.";
  DistillerImpl::DistilledPageData* page_data =
      impl_->GetPageAtIndex(impl_->started_pages_index_[page_num]);
  CHECK(page_data) << "GetPageAtIndex(started_pages_index_[" << page_num
                   << "] (=" << impl_->started_pages_index_[page_num]
                   << ")) returns nullptr. impl_->pages_.size() = "
                   << impl_->pages_.size() << ".";
  page_data->distilled_page_proto = new base::RefCountedData<DistilledPageProto>();
  page_data->page_num = page_num;

  if (distiller_result->has_hw_distiller_result_json()) {
    page_data->distilled_page_proto->data.set_hw_distiller_result_json(
      distiller_result->hw_distiller_result_json());
  }
  if (distiller_result->has_title()) {
    page_data->distilled_page_proto->data.set_title(distiller_result->title());
  }
  page_data->distilled_page_proto->data.set_url(page_url.spec());

  WriteMutableFlatFields(*distiller_result, page_num);
  WriteFlatFields(*distiller_result, page_num, page_url);

  for (int img_num = 0; img_num < distiller_result->content_images_size(); ++img_num) {
    std::string image_id = base::NumberToString(page_num + 1) + "_" +
                           base::NumberToString(img_num);
    impl_->MaybeFetchImage(page_num, image_id,
                           distiller_result->content_images(img_num).url());
  }

  impl_->AddPageIfDone(page_num);
  impl_->DistillNextPage();
}
#endif  // ARKWEB_READER_MODE
}  // namespace dom_distiller