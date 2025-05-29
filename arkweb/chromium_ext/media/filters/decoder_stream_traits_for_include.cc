// Copyright (c) 2025 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

namespace media {

#if BUILDFLAG(ARKWEB_PIP)
void DecoderStreamTraits<DemuxerStream::VIDEO>::PipEnable(
    DecoderType* decoder, bool enable) {
  if (decoder) {
    decoder->PipEnable(enable);
  }
}
#endif
}