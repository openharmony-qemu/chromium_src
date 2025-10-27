// Copyright (c) 2022 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CEF_LIBCEF_BROWSER_NET_DNS_DATA_BASE_IMPL_H_
#define CEF_LIBCEF_BROWSER_NET_DNS_DATA_BASE_IMPL_H_

#include "net/base/address_list.h"

void CacheHostName(const std::string& hostname);

net::AddressList GetAddrList(const std::string& hostname);

#endif  // CEF_LIBCEF_BROWSER_NET_DNS_DATA_BASE_IMPL_H_
