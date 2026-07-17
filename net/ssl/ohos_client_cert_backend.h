// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef NET_SSL_OHOS_CLIENT_CERT_BACKEND_H_
#define NET_SSL_OHOS_CLIENT_CERT_BACKEND_H_

namespace net {

enum class OhosClientCertBackend {
  kOhosUri,
  kHuks,
  kSkf,
};

}  // namespace net

#endif  // NET_SSL_OHOS_CLIENT_CERT_BACKEND_H_
