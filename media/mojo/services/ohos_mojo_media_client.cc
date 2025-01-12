// Copyright (c) 2025 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "media/mojo/services/ohos_mojo_media_client.h"

#include <utility>

#include <memory>
#include "base/logging.h"
#include "base/functional/bind.h"
#include "base/task/single_thread_task_runner.h"
#include "media/base/audio_decoder.h"
#include "media/base/cdm_factory.h"
#include "media/base/media_log.h"
#include "media/base/ohos/ohos_cdm_factory.h"
#include "media/mojo/mojom/media_drm_storage.mojom.h"
#include "media/mojo/mojom/provision_fetcher.mojom.h"
#include "media/mojo/services/ohos_mojo_util.h"
#include "media/filters/ohos/ohos_audio_decoder.h"
using media::ohos_mojo_util::CreateMediaDrmStorage;
using media::ohos_mojo_util::CreateProvisionFetcher;

namespace media {

OHOSMojoMediaClient::OHOSMojoMediaClient() {}

OHOSMojoMediaClient::~OHOSMojoMediaClient() {}

std::unique_ptr<AudioDecoder> OHOSMojoMediaClient::CreateAudioDecoder(
    scoped_refptr<base::SequencedTaskRunner> task_runner,
    std::unique_ptr<MediaLog> media_log) {
  LOG(INFO) << "OHOSMojoMediaClient::CreateAudioDecoder";
  return std::make_unique<OHOSAudioDecoder>(task_runner);
}

std::unique_ptr<CdmFactory> OHOSMojoMediaClient::CreateCdmFactory(
    mojom::FrameInterfaceFactory* frame_interfaces) {
  if (!frame_interfaces) {
    NOTREACHED() << "Host interfaces should be provided when using CDM with "
                 << "OHOSMojoMediaClient";
    return nullptr;
  }

  return std::make_unique<OHOSCdmFactory>(
      base::BindRepeating(&CreateProvisionFetcher, frame_interfaces),
      base::BindRepeating(&CreateMediaDrmStorage, frame_interfaces));
}

}  // namespace media
