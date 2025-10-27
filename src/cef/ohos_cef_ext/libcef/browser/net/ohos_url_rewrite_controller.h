// Copyright (c) 2025 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef ENABLE_REWRITE_URL_UTIL_H_
#define ENABLE_REWRITE_URL_UTIL_H_

class OhosUrlRewriteController {
 public:
  static bool IsRewriteUrlEnabled();
  static void EnableRewriteUrl(bool enable);

 private:
  static bool is_arkweb_rewrite_url_enable_;
};

#endif