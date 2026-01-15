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

#include "log_utils.h"

#include <cstdlib>

#include "net/base/ip_address.h"
#include "net/base/ip_endpoint.h"
#include "url/url_canon_internal.h"
#include "url/url_canon_stdstring.h"

namespace {
void AppendIPv4Address(const unsigned char address[4], url::CanonOutput* output) {
  // Noise the ip addressed.
  for (int i = 0; i < 4; i++) {
    if (i >= 2) {
      output->push_back('*');
      continue;
    }
    char str[16];
    url::_itoa_s(address[i], str, 10);

    for (int ch = 0; str[ch] != 0; ch++)
      output->push_back(str[ch]);

    if (i != 3)
      output->push_back('.');
  }    
}

void AppendIPv6Address(const unsigned char address[16], url::CanonOutput* output) {
  for (int i = 0; i <= 14;) {
    // Consume the next 16 bits from |address|.
    int x = (address[i] << 8) | address[i + 1];
    i += 2;

    // Stringify the 16 bit number (at most requires 4 hex digits).
    char str[5];
    url::_itoa_s(x, str, 16);
    for (int ch = 0; str[ch] != 0; ++ch) {
      // Noise the ip addressed.
      if (i >= 6) {
        output->push_back('*');
      } else {
        output->push_back(str[ch]);
      }
    }

    // Put a colon after each number, except the last.
    if (i < 16)
      output->push_back(':');
  }
}

}  // namespace

namespace net {

std::string LogUtils::AnonymizeIpAddress(const net::IPEndPoint& ip_endpoint) {
  std::string str;
  url::StdStringCanonOutput output(&str);

  if (ip_endpoint.address().IsIPv4()) {
    AppendIPv4Address(ip_endpoint.address().bytes().data(), &output);
  } else if (ip_endpoint.address().IsIPv6()) {
    AppendIPv6Address(ip_endpoint.address().bytes().data(), &output);
  }

  output.Complete();
  return str;
}

}  // namespace net
