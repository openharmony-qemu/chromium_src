// Copyright 2021 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ohos_cef_ext/libcef/browser/page_load_metrics/arkweb_alloy_page_load_metrics_memory_tracker_factory.h"

#include "base/memory/singleton.h"
#include "components/keyed_service/content/browser_context_dependency_manager.h"
#include "components/keyed_service/content/browser_context_keyed_service_factory.h"
#include "components/page_load_metrics/browser/page_load_metrics_memory_tracker.h"

namespace cef {

page_load_metrics::PageLoadMetricsMemoryTracker*
AlloyPageLoadMetricsMemoryTrackerFactory::GetForBrowserContext(
    content::BrowserContext* context) {
  return static_cast<page_load_metrics::PageLoadMetricsMemoryTracker*>(
      GetInstance()->GetServiceForBrowserContext(context, true));
}

AlloyPageLoadMetricsMemoryTrackerFactory*
AlloyPageLoadMetricsMemoryTrackerFactory::GetInstance() {
  return base::Singleton<AlloyPageLoadMetricsMemoryTrackerFactory>::get();
}

AlloyPageLoadMetricsMemoryTrackerFactory::
    AlloyPageLoadMetricsMemoryTrackerFactory()
    : BrowserContextKeyedServiceFactory(
          "PageLoadMetricsMemoryTracker",
          BrowserContextDependencyManager::GetInstance()) {}

std::unique_ptr<KeyedService>
AlloyPageLoadMetricsMemoryTrackerFactory::BuildServiceInstanceForBrowserContext(
    content::BrowserContext* context) const {
  return std::make_unique<page_load_metrics::PageLoadMetricsMemoryTracker>();
}

content::BrowserContext*
AlloyPageLoadMetricsMemoryTrackerFactory::GetBrowserContextToUse(
    content::BrowserContext* context) const {
  return context;
}

}  // namespace cef
