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

#include "libcef/browser/dom_distiller/oh_distiller_utils.h"

#include <memory>
#include <string>
#include <utility>

#include "base/containers/lru_cache.h"
#include "base/json/json_writer.h"
#include "base/logging.h"
#include "base/strings/string_number_conversions.h"
#include "base/strings/stringprintf.h"
#include "base/values.h"
#include "components/dom_distiller/core/proto/distilled_article.pb.h"
#include "components/dom_distiller/core/proto/distilled_page.pb.h"

namespace oh_dom_distiller {

namespace {

// JSON field name constants
constexpr char kPageCount[] = "pageCount";
constexpr char kBookName[] = "bookName";
constexpr char kChapterName[] = "chapterName";
constexpr char kDescription[] = "description";
constexpr char kCoverUrl[] = "coverUrl";
constexpr char kAuthor[] = "author";
constexpr char kPageContent[] = "pageContent";
constexpr char kCategory[] = "category";
constexpr char kLastChapterName[] = "lastChapterName";
constexpr char kLastChapterUrl[] = "lastChapterUrl";
constexpr char kStartReadLink[] = "startReadLink";
constexpr char kFinishedStatus[] = "finishedStatus";
constexpr char kType[] = "type";
constexpr char kContent[] = "content";
constexpr char kCatalog[] = "catalog";
constexpr char kNextPageUrl[] = "nextPageUrl";
constexpr char kPrevPageUrl[] = "prevPageUrl";
constexpr char kCatalogPageUrl[] = "catalogPageUrl";
constexpr char kLatestChapters[] = "latestChapters";
constexpr char kAllChapters[] = "allChapters";
constexpr char kChapterUrl[] = "chapterUrl";
constexpr char kMorePageUrl[] = "morePageUrl";
constexpr char kPageName[] = "pageName";
constexpr char kPageUrl[] = "pageUrl";
constexpr char kSelected[] = "selected";
constexpr char kPageSelectList[] = "pageSelectList";
constexpr char kCatalogPagination[] = "catalogPagination";
constexpr char kTitle[] = "title";
constexpr char kTotalTime[] = "total_time";
constexpr char kDistillTime[] = "distill_time";
constexpr char kTimingInfo[] = "timingInfo";
constexpr char kCurrentPageUrl[] = "currentPageUrl";
constexpr char kResultList[] = "resultList";

bool FillResultFields(
    const dom_distiller::DistilledPageProto_HwReadExtendFields& flat,
    base::Value::Dict& dict) {
  dict.Set(kResultCode, 0);
  if (flat.has_distilled_result()) {
    const auto& res = flat.distilled_result();
    if (res.has_code()) {
      dict.Set(kResultCode, res.code());
    }
    if (res.has_message()) {
      dict.Set(kResultMessage, res.message());
    }
    return res.has_code();
  }
  return false;
}

void FillBasicInfoFields(
    const dom_distiller::DistilledPageProto_HwReadExtendFields& flat,
    base::Value::Dict& dict) {
  if (flat.has_book_name()) {
    dict.Set(kBookName, flat.book_name());
  }
  if (flat.has_chapter_name()) {
    dict.Set(kChapterName, flat.chapter_name());
  }
  if (flat.has_description()) {
    dict.Set(kDescription, flat.description());
  }
  if (flat.has_img_url()) {
    dict.Set(kCoverUrl, flat.img_url());
  }
  if (flat.has_author()) {
    dict.Set(kAuthor, flat.author());
  }
  if (flat.has_content()) {
    dict.Set(kPageContent, flat.content());
  }
  if (flat.has_category()) {
    dict.Set(kCategory, flat.category());
  }
  if (flat.has_last_chapter_name()) {
    dict.Set(kLastChapterName, flat.last_chapter_name());
  }
  if (flat.has_last_chapter_url()) {
    dict.Set(kLastChapterUrl, flat.last_chapter_url());
  }
  if (flat.has_start_read_link()) {
    dict.Set(kStartReadLink, flat.start_read_link());
  }
  if (flat.has_finished_status()) {
    dict.Set(kFinishedStatus, flat.finished_status());
  }
}

void FillContentInfoFields(
    const dom_distiller::DistilledPageProto_HwReadExtendFields& flat,
    bool has_result,
    base::Value::Dict& dict) {
  if (flat.has_content_info()) {
    dict.Set(kType, kContent);
    const auto& info = flat.content_info();
    if (info.has_next_page()) {
      dict.Set(kNextPageUrl, info.next_page());
    }
    if (info.has_prev_page()) {
      dict.Set(kPrevPageUrl, info.prev_page());
    }
    if (info.has_catalog_page()) {
      dict.Set(kCatalogPageUrl, info.catalog_page());
    }
  }
  if (!has_result && ((flat.has_content_info() && !flat.has_content()) ||
                      (flat.has_content() && flat.content().empty()))) {
    dict.Set(kResultCode, kNoContentErrorCode);
    dict.Set(kResultMessage, kErrorMsgNoContent);
  }
}

bool FillPaginationInfo(const dom_distiller::DistilledPageProto_HwReadExtendFields_CatalogPageInfo& cat,
  base::Value::Dict& dict) {
  bool has_pagination = false;
  if (!cat.has_pagination_info()) {
    LOG(WARNING) << __func__ << "[Distiller] no pagination_info";
    return;
  }
  base::Value::Dict pag;
  const auto& p = cat.pagination_info();
  if (p.has_next_catalog_page()) {
    pag.Set(kNextPageUrl, p.next_catalog_page());
    has_pagination = !p.next_catalog_page().empty();
  }
  if (p.has_prev_catalog_page()) {
    pag.Set(kPrevPageUrl, p.prev_catalog_page());
    has_pagination = !p.prev_catalog_page().empty();
  }
  if (p.has_whole_catalog_page()) {
    pag.Set(kMorePageUrl, p.whole_catalog_page());
    has_pagination = !p.whole_catalog_page().empty();
  }

  base::Value::List pages;
  for (const auto& cp : p.all_catalog_pages()) {
    base::Value::Dict pi;
    pi.Set(kPageName, cp.page_name());
    pi.Set(kPageUrl, cp.page_url());
    if (cp.has_selected() && cp.selected()) {
      pi.Set(kSelected, cp.selected());
    }
    pages.Append(std::move(pi));
    has_pagination = true;
  }
  if (!pages.empty()) {
    pag.Set(kPageSelectList, std::move(pages));
  }
  dict.Set(kCatalogPagination, std::move(pag));
  return has_pagination;
}

void FillCatalogInfoFields(
    const dom_distiller::DistilledPageProto_HwReadExtendFields& flat,
    bool has_result,
    base::Value::Dict& dict) {
  if (!flat.has_catalog_info()) {
    LOG(WARNING) << __func__ << "[Distiller] no catalog_info";
    return;
  }
  dict.Set(kType, kCatalog);
  const auto& cat = flat.catalog_info();

  base::Value::List latest;
  base::Value::List all;
  for (const auto& ch : cat.latest_chapters()) {
    base::Value::Dict ci;
    ci.Set(kChapterName, ch.chapter_name());
    ci.Set(kChapterUrl, ch.chapter_url());
    latest.Append(std::move(ci));
  }
  if (!latest.empty()) {
    dict.Set(kLatestChapters, std::move(latest));
  }

  for (const auto& ch : cat.all_chapters()) {
    base::Value::Dict ci;
    ci.Set(kChapterName, ch.chapter_name());
    ci.Set(kChapterUrl, ch.chapter_url());
    all.Append(std::move(ci));
  }
  if (!all.empty()) {
    dict.Set(kAllChapters, std::move(all));
  }

  bool has_pagination = FillPaginationInfo(cat, dict);
  if (!has_result && cat.all_chapters_size() == 0 && !has_pagination) {
    dict.Set(kResultCode, kNoChaptersErrorCode);
    dict.Set(kResultMessage, kErrorMsgNoChapters);
  }
}

void FillTimingInfoFields(
    const dom_distiller::DistilledPageProto_HwReadExtendFields& flat,
    base::Value::Dict& dict) {
  if (flat.has_timing_info()) {
    const auto& t = flat.timing_info();
    base::Value::Dict timing;
    if (t.has_total_time()) {
      std::string total_time = base::StringPrintf("%.2f", t.total_time());
      timing.Set(kTotalTime, total_time);
    }
    if (t.has_distill_time()) {
      std::string distill_time = base::StringPrintf("%.2f", t.distill_time());
      timing.Set(kDistillTime, distill_time);
    }
    dict.Set(kTimingInfo, std::move(timing));
  }
}

base::Value::Dict BuildPageContent(
    const dom_distiller::DistilledPageProto& page) {
  base::Value::Dict dict;
  const auto& flat = page.flat_fields();
  bool has_result_code = FillResultFields(flat, dict);
  FillBasicInfoFields(flat, dict);
  FillContentInfoFields(flat, has_result_code, dict);
  FillCatalogInfoFields(flat, has_result_code, dict);
  FillTimingInfoFields(flat, dict);

  if (page.has_title()) {
    dict.Set(kTitle, page.title());
  }
  dict.Set(kCurrentPageUrl, page.url());

  const int result_code = dict.FindInt(kResultCode).value_or(1);
  auto* result_message = dict.FindString(kResultMessage);
  auto* next_page_url = dict.FindString(kNextPageUrl);
  auto* prev_page_url = dict.FindString(kPrevPageUrl);
  auto* catalog_page_url = dict.FindString(kCatalogPageUrl);
  auto* content = dict.FindString(kPageContent);
  auto* latest_chapters = dict.FindList(kLatestChapters);
  auto* all_chapters = dict.FindList(kAllChapters);
  auto* type = dict.FindString(kType);

  std::string json;
  if (auto* timing_info = dict.FindDict(kTimingInfo)) {
    base::JSONWriter::Write(*timing_info, &json);
  }

  std::string pagination_str;
  if (auto* pagination = dict.FindDict(kCatalogPagination)) {
    auto* next_page = pagination->FindString(kNextPageUrl);
    auto* prev_page = pagination->FindString(kPrevPageUrl);
    auto* more_page = pagination->FindString(kMorePageUrl);
    auto* page_select = pagination->FindList(kPageSelectList);
    pagination_str = base::StringPrintf(
        "%d %d %d %zu", (next_page && !next_page->empty()),
        (prev_page && !prev_page->empty()), (more_page && !more_page->empty()),
        (page_select ? page_select->size() : 0));
  }

  LOG(INFO) << "[Distiller] DistillResult type:" << (type ? *type : "")
            << " resultCode:" << result_code << " "
            << (result_message ? *result_message : "")
            << " contents:" << (content ? (int)content->size() : -1) << " "
            << (next_page_url ? !next_page_url->empty() : false) << " "
            << (prev_page_url ? !prev_page_url->empty() : false) << " "
            << (catalog_page_url ? !catalog_page_url->empty() : false)
            << " chapters:"
            << (latest_chapters ? (int)latest_chapters->size() : -1) << " "
            << (all_chapters ? (int)all_chapters->size() : -1) << " "
            << pagination_str << " timing_info:" << json;
  return dict;
}

base::Value::List BuildDistilledPageList(
    const dom_distiller::DistilledArticleProto* proto) {
  base::Value::List list;
  for (size_t i = 0; i < proto->pages_size(); ++i) {
    list.Append(BuildPageContent(proto->pages(i)));
  }
  return list;
}

}  // namespace

std::unique_ptr<std::string> FormatDistilledArticleProtoToJsonData(
    const dom_distiller::DistilledArticleProto* article_proto,
    const GURL& url) {

  auto result = std::make_unique<std::string>();
  base::Value::Dict json;

  if (!article_proto || article_proto->pages_size() == 0) {
    json.Set(kPageCount, 0);
    json.Set(kResultCode, kNoPageErrorCode);
    json.Set(kResultMessage, kErrorMsgNoPage);
    base::JSONWriter::Write(std::move(json), result.get());
    LOG(INFO) << "[Distiller] DistillResult no pages";
    return result;
  }

  json.Set(kPageCount, article_proto->pages_size());
  base::Value::List page_list = BuildDistilledPageList(article_proto);
  if (!page_list.empty()) {
    json.Set(kResultList, std::move(page_list));
  }

  base::JSONWriter::Write(std::move(json), result.get());
  return result;
}

}  // namespace oh_dom_distiller
