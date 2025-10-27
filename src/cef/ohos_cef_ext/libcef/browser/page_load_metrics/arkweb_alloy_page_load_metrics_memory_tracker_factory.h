// Copyright 2021 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CEF_LIBCEF_BROWSER_PAGE_LOAD_METRICS_ALLOY_PAGE_LOAD_METRICS_MEMORY_TRACKER_FACTORY_H_
#define CEF_LIBCEF_BROWSER_PAGE_LOAD_METRICS_ALLOY_PAGE_LOAD_METRICS_MEMORY_TRACKER_FACTORY_H_

#include "components/keyed_service/content/browser_context_keyed_service_factory.h"

namespace page_load_metrics {
class PageLoadMetricsMemoryTracker;
}  // namespace page_load_metrics

namespace cef {

class AlloyPageLoadMetricsMemoryTrackerFactory
    : public BrowserContextKeyedServiceFactory {
 public:
  static page_load_metrics::PageLoadMetricsMemoryTracker* GetForBrowserContext(
      content::BrowserContext* context);

  static AlloyPageLoadMetricsMemoryTrackerFactory* GetInstance();

  AlloyPageLoadMetricsMemoryTrackerFactory();

 private:
  KeyedService* BuildServiceInstanceFor(
      content::BrowserContext* context) const override;

  content::BrowserContext* GetBrowserContextToUse(
      content::BrowserContext* context) const override;
};

}  // namespace cef

#endif  // CEF_LIBCEF_BROWSER_PAGE_LOAD_METRICS_ALLOY_PAGE_LOAD_METRICS_MEMORY_TRACKER_FACTORY_H_
