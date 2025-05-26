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

#ifndef THIRD_PARTY_BLINK_RENDERER_PLATFORM_LOADER_FETCH_URL_LOADER_SYNC_LOAD_CONTEXT_H_
#error "must be in include form THIRD_PARTY_BLINK_RENDERER_PLATFORM_LOADER_FETCH_URL_LOADER_SYNC_LOAD_CONTEXT_H_"
#endif

#include "arkweb/chromium_ext/base/process/process_handle_posix_ex.h"
 
#if BUILDFLAG(ARKWEB_RESOURCE_INTERCEPTION)
#include "base/trace_event/trace_event.h"
#endif
 
namespace blink {

void SyncLoadContext::BindRemote(content::RenderThread* render_thread) {
  if (report_manager_ || !render_thread) {
    return;
  }
  render_thread->BindHostReceiver(report_manager_.BindNewPipeAndPassReceiver());
}
 
static void SyncLoadContextExt(raw_ptr<SyncLoadContext> obj, content::RenderThread* render_thread) {
  LOG(DEBUG) << "SyncLoadContext pid=" << base::GetCurrentProcId()
             << ", main thread tid=" << base::GetCurrentRealPid()
             << ", real tid=" << base::PlatformThread::CurrentRealId();
  obj->BindRemote(render_thread);
 
  if (obj->report_manager_) {
    std::vector<int> tids;
    tids.push_back(base::GetCurrentRealPid());
    tids.push_back(base::PlatformThread::CurrentRealId());
    obj->report_manager_->AddRtg(tids);
    obj->report_manager_->FetchBegin();
  }
}
 
void SyncLoadContext::OnTransferDataWithSharedMemory(
    base::ReadOnlySharedMemoryRegion region,
    uint64_t buffer_size) {
  TRACE_EVENT0("loading", "SyncLoadContext::OnTransferDataWithSharedMemory");
  LOG(DEBUG)
      << "shared-memory SyncLoadContext::OnTransferDataWithSharedMemory+++, "
         "buffer_size:"
      << buffer_size;
  if (!region.IsValid()) {
    LOG(ERROR) << "shared-memory region is invalid";
    response_->error_code = net::ERR_FAILED;
    CompleteRequest();
    return;
  }
  base::ReadOnlySharedMemoryMapping mapping = region.Map();
  if (!mapping.IsValid()) {
    LOG(ERROR) << "shared-memory Read-only shared memory mapping is invalid";
    response_->error_code = net::ERR_FAILED;
    CompleteRequest();
    return;
  }
  const char* buffer = mapping.GetMemoryAs<char>();
  size_t buffer_len = static_cast<size_t>(buffer_size);
  LOG(DEBUG) << "shared-memory "
                "SyncLoadContext::OnTransferDataWithSharedMemory, buffer_len:"
             << buffer_len;
  if (!response_->data) {
    response_->data = SharedBuffer::Create(buffer, buffer_len);
  } else {
    response_->data->Append(buffer, buffer_len);
  }
  auto status = network::URLLoaderCompletionStatus(net::OK);
  status.completion_time = base::TimeTicks::Now();
  status.encoded_data_length = buffer_len;
  status.encoded_body_length = buffer_len;
  // We don't support decoders, so use the same value.
  status.decoded_body_length = buffer_len;
  request_completed_ = true;
  response_->error_code = status.error_code;
  response_->extended_error_code = status.extended_error_code;
  response_->resolve_error_info = status.resolve_error_info;
  response_->should_collapse_initiator = status.should_collapse_initiator;
  response_->cors_error = status.cors_error_status;
  response_->head->encoded_data_length = status.encoded_data_length;
  DCHECK_GE(status.encoded_body_length, 0);
  response_->head->encoded_body_length =
      network::mojom::EncodedBodyLength::New(status.encoded_body_length);
  CompleteRequest();
  LOG(DEBUG)
      << "shared-memory SyncLoadContext::OnTransferDataWithSharedMemory---, "
         "buffer_size:"
      << buffer_size;
}

}