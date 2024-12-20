// Copyright 2016 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "media/base/media_content_type.h"

namespace media {

namespace {
const int kMinimumContentDurationSecs = 5;
#if defined(OHOS_MEDIA_POLICY)
const int kSnippetContentDurationSecs = 60;
#endif // defined(OHOS_MEDIA_POLICY)
}  // anonymous namespace

MediaContentType DurationToMediaContentType(base::TimeDelta duration) {
#if defined(OHOS_MEDIA_POLICY)
  if (duration > base::Seconds(kMinimumContentDurationSecs) &&
      duration <= base::Seconds(kSnippetContentDurationSecs)) {
       return MediaContentType::Snippet;
  }
#endif // defined(OHOS_MEDIA_POLICY)
  // A zero duration indicates that the duration is unknown. "Persistent" type
  // should be used in this case.
  return (duration.is_zero() ||
          duration > base::Seconds(kMinimumContentDurationSecs))
             ? MediaContentType::Persistent
             : MediaContentType::Transient;
}

}  // namespace media
