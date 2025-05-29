// Copyright (c) 2025 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "media/mojo/services/ohos_mojo_util.h"

#include "media/mojo/services/mojo_media_drm_storage.h"
#include "mojo/public/cpp/bindings/pending_remote.h"

namespace media {
namespace ohos_mojo_util {

std::unique_ptr<ProvisionFetcher> CreateProvisionFetcher(
    media::mojom::FrameInterfaceFactory* frame_interfaces) {
  LOG(INFO) << "[DRM]" << __func__;
  DCHECK(frame_interfaces);
  mojo::PendingRemote<mojom::ProvisionFetcher> provision_fetcher;
  frame_interfaces->CreateProvisionFetcher(
      provision_fetcher.InitWithNewPipeAndPassReceiver());
  LOG(INFO) << "[DRM]" << __func__;
  return std::make_unique<MojoProvisionFetcher>(std::move(provision_fetcher));
}

std::unique_ptr<MediaDrmStorage> CreateMediaDrmStorage(
    media::mojom::FrameInterfaceFactory* frame_interfaces) {
  LOG(INFO) << "[DRM]" << __func__;
  DCHECK(frame_interfaces);
  mojo::PendingRemote<mojom::MediaDrmStorage> media_drm_storage;
  frame_interfaces->BindEmbedderReceiver(mojo::GenericPendingReceiver(
      media_drm_storage.InitWithNewPipeAndPassReceiver()));
  LOG(INFO) << "[DRM]" << __func__;
  return std::make_unique<MojoMediaDrmStorage>(std::move(media_drm_storage));
}

}  // namespace ohos_mojo_util
}  // namespace media
