// Copyright 2016 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_PUBLIC_WEB_WEB_ASSOCIATED_URL_LOADER_CLIENT_H_
#define THIRD_PARTY_BLINK_PUBLIC_WEB_WEB_ASSOCIATED_URL_LOADER_CLIENT_H_

#include "base/containers/span.h"

#if BUILDFLAG(IS_ARKWEB_EXT)
#include "arkweb/ohos_nweb_ex/build/features/features.h"
#endif // IS_ARKWEB_EXT

namespace blink {

class WebURL;
class WebURLResponse;
struct WebURLError;

class WebAssociatedURLLoaderClient {
 public:
  virtual bool WillFollowRedirect(const WebURL& new_url,
                                  const WebURLResponse& redirect_response) {
    return true;
  }
  virtual void DidSendData(uint64_t bytes_sent,
                           uint64_t total_bytes_to_be_sent) {}
  virtual void DidReceiveResponse(const WebURLResponse&) {}
  virtual void DidDownloadData(uint64_t data_length) {}
  virtual void DidReceiveData(base::span<const char> data) {}
  virtual void DidFinishLoading() {}
  virtual void DidFail(const WebURLError&) {}

#if BUILDFLAG(ARKWEB_EXT_VIDEO_LOAD_OPTIMIZATION)
  virtual void DidReStart() {}
#endif // ARKWEB_EXT_VIDEO_LOAD_OPTIMIZATION

 protected:
  virtual ~WebAssociatedURLLoaderClient() = default;
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_PUBLIC_WEB_WEB_ASSOCIATED_URL_LOADER_CLIENT_H_
