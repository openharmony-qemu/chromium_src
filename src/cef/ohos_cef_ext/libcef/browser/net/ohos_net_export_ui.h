// Copyright 2024 The Huawei Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Based on net_export_ui.cc originally written by
// Copyright 2013 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CEF_LIBCEF_BROWSER_NET_OHOS_NET_EXPORT_UI_H_
#define CEF_LIBCEF_BROWSER_NET_OHOS_NET_EXPORT_UI_H_

#include "content/public/browser/web_ui_controller.h"

class OhosNetExportUI : public content::WebUIController {
 public:
  OhosNetExportUI(const OhosNetExportUI&) = delete;
  OhosNetExportUI& operator=(const OhosNetExportUI&) = delete;

  explicit OhosNetExportUI(content::WebUI* web_ui);
  ~OhosNetExportUI() override;
};

#endif  // CEF_LIBCEF_BROWSER_NET_OHOS_NET_EXPORT_UI_H_
