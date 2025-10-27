// Copyright (c) 2022 Huawei Device Co., Ltd.
// Copyright 2018 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef OH_JAVASCRIPT_INJECTOR_H
#define OH_JAVASCRIPT_INJECTOR_H

#include "cef/ohos_cef_ext/include/arkweb_client_ext.h"
#include "content/public/browser/web_contents_user_data.h"
#include "libcef/browser/browser_info.h"

namespace NWEB {
class OhGinJavascriptBridgeDispatcherHost;

class OhJavascriptInjector
    : public content::WebContentsUserData<OhJavascriptInjector> {
 public:
  OhJavascriptInjector(content::WebContents* web_contents,
                       CefRefPtr<ArkWebClientExt> client);

  OhJavascriptInjector(const OhJavascriptInjector&) = delete;
  OhJavascriptInjector& operator=(const OhJavascriptInjector&) = delete;
  ~OhJavascriptInjector();

  void AddNativeInterface(const std::string& object_name,
                          const std::vector<std::string> method_list,
                          const int32_t object_id,
                          bool is_async,
                          const std::string& permission);
  void AddInterface(const std::string& object_name,
                    const std::vector<std::string> method_list,
                    const std::vector<std::string> async_method_list,
                    const int32_t object_id,
                    const std::string& permission);
  void RemoveInterface(const std::string& object_name,
                       const std::vector<std::string> method_list);
  void DoCallH5Function(int32_t routing_id,
                        int32_t h5_object_id,
                        const std::string& h5_method_name,
                        const std::vector<CefRefPtr<CefValue>>& args);
  void SetLastCallingFrameUrl(const std::string& url) {
    last_calling_frame_url_ = url;
  }
  std::string GetLastCallingFrameUrl() { return last_calling_frame_url_; }

 private:
  friend class content::WebContentsUserData<OhJavascriptInjector>;

  scoped_refptr<OhGinJavascriptBridgeDispatcherHost>
      javascript_bridge_dispatcher_host_;
  std::string last_calling_frame_url_;
  WEB_CONTENTS_USER_DATA_KEY_DECL();
};
}  // namespace NWEB
#endif
