// Copyright (c) 2022 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CONTENT_RENDERER_MEDIA_OHOS_MEDIA_PLAYER_RENDERER_CLIENT_H_
#define CONTENT_RENDERER_MEDIA_OHOS_MEDIA_PLAYER_RENDERER_CLIENT_H_

#include <memory>

#include "base/functional/callback.h"
#include "base/memory/weak_ptr.h"
#include "base/task/single_thread_task_runner.h"
#include "content/common/content_export.h"
#include "media/base/native_texture_wrapper.h"
#include "media/base/media_resource.h"
#include "media/base/renderer.h"
#include "media/base/renderer_client.h"
#include "media/base/video_renderer_sink.h"
#include "media/base/video_util.h"
#include "media/base/video_util_ext.h"
#include "media/mojo/clients/mojo_renderer.h"
#include "media/mojo/clients/mojo_renderer_wrapper.h"
#include "media/mojo/mojom/renderer_extensions.mojom.h"
#include "mojo/public/cpp/bindings/pending_receiver.h"
#include "mojo/public/cpp/bindings/pending_remote.h"
#include "mojo/public/cpp/bindings/receiver.h"
#include "mojo/public/cpp/bindings/remote.h"

namespace content {


}  // namespace content

#endif  // CONTENT_RENDERER_MEDIA_OHOS_MEDIA_PLAYER_RENDERER_CLIENT_H_