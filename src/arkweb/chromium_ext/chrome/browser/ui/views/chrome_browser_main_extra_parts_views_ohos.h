/*
 * Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#ifndef CHROME_BROWSER_UI_VIEWS_CHROME_BROWSER_MAIN_EXTRA_PARTS_VIEWS_OHOS_H_
#define CHROME_BROWSER_UI_VIEWS_CHROME_BROWSER_MAIN_EXTRA_PARTS_VIEWS_OHOS_H_

#include "chrome/browser/ui/views/chrome_browser_main_extra_parts_views.h"

class ChromeBrowserMainExtraPartsViewsOHOS
    : public ChromeBrowserMainExtraPartsViews {
 public:
  ChromeBrowserMainExtraPartsViewsOHOS();

  ChromeBrowserMainExtraPartsViewsOHOS(
      const ChromeBrowserMainExtraPartsViewsOHOS&) = delete;
  ChromeBrowserMainExtraPartsViewsOHOS& operator=(
      const ChromeBrowserMainExtraPartsViewsOHOS&) = delete;

  ~ChromeBrowserMainExtraPartsViewsOHOS() override;

  // Overridden from ChromeBrowserMainExtraParts:
  void ToolkitInitialized() override;
};

#endif  // CHROME_BROWSER_UI_VIEWS_CHROME_BROWSER_MAIN_EXTRA_PARTS_VIEWS_OHOS_H_
