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

#include "libcef/browser/dom_distiller/oh_dom_distiller_service_factory.h"

#include <utility>

#include "base/logging.h"
#include "base/task/sequenced_task_runner.h"
#include "base/task/thread_pool.h"
#include "build/build_config.h"
#include "cef/libcef/browser/browser_context.h"
#include "chrome/browser/profiles/profile.h"
#include "components/dom_distiller/content/browser/distiller_page_web_contents.h"
#include "components/dom_distiller/core/article_entry.h"
#include "components/dom_distiller/core/distiller.h"
#include "components/keyed_service/content/browser_context_dependency_manager.h"
#include "content/public/browser/browser_context.h"
#include "content/public/browser/storage_partition.h"
#include "services/network/public/cpp/shared_url_loader_factory.h"

namespace oh_dom_distiller {

OhDomDistillerContextKeyedService::OhDomDistillerContextKeyedService(
    std::unique_ptr<dom_distiller::DistillerFactory> distiller_factory,
    std::unique_ptr<dom_distiller::DistillerPageFactory> distiller_page_factory,
    std::unique_ptr<dom_distiller::DistilledPagePrefs> distilled_page_prefs,
    std::unique_ptr<dom_distiller::DistillerUIHandle> distiller_ui_handle)
    : DomDistillerService(std::move(distiller_factory),
                          std::move(distiller_page_factory),
                          std::move(distilled_page_prefs),
                          std::move(distiller_ui_handle)) {}

// static
OhDomDistillerServiceFactory* OhDomDistillerServiceFactory::GetInstance() {
  static base::NoDestructor<OhDomDistillerServiceFactory> instance;
  return instance.get();
}

// static
OhDomDistillerContextKeyedService*
OhDomDistillerServiceFactory::GetForBrowserContext(
    content::BrowserContext* context) {
  return static_cast<OhDomDistillerContextKeyedService*>(
      GetInstance()->GetServiceForBrowserContext(context, true));
}

OhDomDistillerServiceFactory::OhDomDistillerServiceFactory()
    : BrowserContextKeyedServiceFactory(
          "DomDistillerService",
          BrowserContextDependencyManager::GetInstance()) {}

OhDomDistillerServiceFactory::~OhDomDistillerServiceFactory() = default;

std::unique_ptr<KeyedService>
OhDomDistillerServiceFactory::BuildServiceInstanceForBrowserContext(
    content::BrowserContext* context) const {
  scoped_refptr<base::SequencedTaskRunner> background_task_runner =
      base::ThreadPool::CreateSequencedTaskRunner(
          {base::MayBlock(), base::TaskPriority::BEST_EFFORT});

  std::unique_ptr<dom_distiller::DistillerPageFactory> distiller_page_factory(
      new dom_distiller::DistillerPageWebContentsFactory(context));
  std::unique_ptr<dom_distiller::DistillerURLFetcherFactory> distiller_url_fetcher_factory(
      new dom_distiller::DistillerURLFetcherFactory(
          context->GetDefaultStoragePartition()
              ->GetURLLoaderFactoryForBrowserProcess()));

  dom_distiller::proto::DomDistillerOptions options;
  // Options for pagination algorithm:
  // - "next": detect anchors with "next" text
  // - "pagenum": detect anchors with numeric page numbers
  // Default is "next".
  options.set_pagination_algo("next");
  options.set_extract_text_only(true);
  std::unique_ptr<dom_distiller::DistillerFactory> distiller_factory(
      new dom_distiller::DistillerFactoryImpl(
          std::move(distiller_url_fetcher_factory), options));
  auto cef_browser_context = CefBrowserContext::FromBrowserContext(context);
  if (cef_browser_context == nullptr || cef_browser_context->AsProfile() == nullptr) {
    LOG(ERROR) << __func__ << " [Distiller] cef_browser_context or AsProfile is null";
    return nullptr;
  }
  std::unique_ptr<dom_distiller::DistilledPagePrefs> distilled_page_prefs(
      new dom_distiller::DistilledPagePrefs(cef_browser_context->AsProfile()->GetPrefs()));
  std::unique_ptr<dom_distiller::DistillerUIHandle> distiller_ui_handle;

  return std::make_unique<OhDomDistillerContextKeyedService>(
      std::move(distiller_factory), std::move(distiller_page_factory),
      std::move(distilled_page_prefs), std::move(distiller_ui_handle));
}

} // namespace oh_dom_distiller
