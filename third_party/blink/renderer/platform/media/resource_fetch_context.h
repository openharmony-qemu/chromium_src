// Copyright 2017 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_PLATFORM_MEDIA_RESOURCE_FETCH_CONTEXT_H_
#define THIRD_PARTY_BLINK_RENDERER_PLATFORM_MEDIA_RESOURCE_FETCH_CONTEXT_H_

#include <memory>

namespace blink {

class WebAssociatedURLLoader;
struct WebAssociatedURLLoaderOptions;

class ResourceFetchContext {
 public:
  virtual ~ResourceFetchContext() = default;

  virtual std::unique_ptr<WebAssociatedURLLoader> CreateUrlLoader(
      const WebAssociatedURLLoaderOptions& options) = 0;

#if BUILDFLAG(ARKWEB_EXT_VIDEO_LOAD_OPTIMIZATION)
  virtual std::unique_ptr<WebAssociatedURLLoader> CreateVideoUrlLoader(
      const WebAssociatedURLLoaderOptions& options) {
    return nullptr;
  }
#endif // ARKWEB_EXT_VIDEO_LOAD_OPTIMIZATION
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_PLATFORM_MEDIA_RESOURCE_FETCH_CONTEXT_H_
