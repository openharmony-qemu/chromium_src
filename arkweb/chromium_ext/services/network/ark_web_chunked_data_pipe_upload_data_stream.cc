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

#include "arkweb/build/features/features.h"
#include "base/functional/bind.h"
#include "services/network/chunked_data_pipe_upload_data_stream.h"
#include "services/network/public/mojom/chunked_data_pipe_getter.mojom.h"

namespace network {

void ChunkedDataPipeUploadDataStream::ArkWebInitInternal() {
#if BUILDFLAG(ARKWEB_SCHEME_HANDLER)
  chunked_data_pipe_getter_.set_disconnect_handler(
      base::BindOnce(&ChunkedDataPipeUploadDataStream::OnDataPipeGetterClosed,
                     base::Unretained(this)));
  chunked_data_pipe_getter_->GetSize(
      base::BindOnce(&ChunkedDataPipeUploadDataStream::OnSizeReceived,
                     base::Unretained(this)));
#endif  // BUILDFLAG(ARKWEB_SCHEME_HANDLER)
}

#if BUILDFLAG(ARKWEB_SCHEME_HANDLER)
mojo::PendingRemote<mojom::ChunkedDataPipeGetter>
ChunkedDataPipeUploadDataStream::ReleaseChunkedDataPipeGetter() {
  return chunked_data_pipe_getter_.Unbind();
}
#endif  // BUILDFLAG(ARKWEB_SCHEME_HANDLER)

}  // namespace network
