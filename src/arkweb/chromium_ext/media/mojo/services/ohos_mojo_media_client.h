// Copyright (c) 2025 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MEDIA_MOJO_SERVICES_OHOS_MOJO_MEDIA_CLIENT_H_
#define MEDIA_MOJO_SERVICES_OHOS_MOJO_MEDIA_CLIENT_H_

#include <memory>

#include "base/task/sequenced_task_runner.h"
#include "media/mojo/services/mojo_media_client.h"

namespace media {

class OHOSMojoMediaClient final : public MojoMediaClient {
 public:
  OHOSMojoMediaClient();

  OHOSMojoMediaClient(const OHOSMojoMediaClient&) = delete;
  OHOSMojoMediaClient& operator=(const OHOSMojoMediaClient&) = delete;

  ~OHOSMojoMediaClient() override;

  std::unique_ptr<AudioDecoder> CreateAudioDecoder(
      scoped_refptr<base::SequencedTaskRunner> task_runner,
      std::unique_ptr<MediaLog> media_log) override;

  std::unique_ptr<CdmFactory> CreateCdmFactory(
      mojom::FrameInterfaceFactory* frame_interfaces) override;
};

}  // namespace media

#endif  // MEDIA_MOJO_SERVICES_OHOS_MOJO_MEDIA_CLIENT_H_
