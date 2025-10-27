// Copyright 2024 The Chromium Embedded Framework Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CEF_LIBCEF_REPORT_MANAGER_H_
#define CEF_LIBCEF_REPORT_MANAGER_H_

#include <vector>

#include "cef/libcef/common/mojom/cef.mojom.h"
#include "mojo/public/cpp/bindings/remote.h"
#include "services/service_manager/public/cpp/binder_registry.h"

namespace blink {
class AssociatedInterfaceRegistry;
}

namespace content {
class RenderProcessHost;
}

class CefReportManager : public cef::mojom::ReportManager {
 public:
  explicit CefReportManager(int render_process_id);

  CefReportManager(const CefReportManager&) = delete;
  CefReportManager& operator=(const CefReportManager&) = delete;

  ~CefReportManager() override;

  static void ExposeInterfacesToRenderer(
      service_manager::BinderRegistry* registry,
      blink::AssociatedInterfaceRegistry* associated_registry,
      content::RenderProcessHost* host);

  void ReportKeyThread(int status,
                       int process_id,
                       int thread_id,
                       int role) override;
  void AddRtg(const std::vector<int>& tids) override;
  void FetchBegin() override;
  void FetchEnd() override;

 private:
  // The process ID of the renderer.
  const int render_process_id_;
};

#endif  // CEF_LIBCEF_REPORT_MANAGER_H_
