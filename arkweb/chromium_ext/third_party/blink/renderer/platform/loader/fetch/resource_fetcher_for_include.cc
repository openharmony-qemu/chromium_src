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

#if BUILDFLAG(ARKWEB_ADBLOCK)
bool ResourceFinishAsError(
    FetchContext& context,
    Resource* resource,
    ResourceRequest& last_resource_request,
    FetchParameters& params,
    scoped_refptr<base::SingleThreadTaskRunner> freezable_task_runner) {
  std::optional<ResourceRequestBlockedReason> blocked_reason =
      context.CanRequest(resource->GetType(), last_resource_request,
                         last_resource_request.Url(), params.Options(),
                         ReportingDisposition::kReport,
                         last_resource_request.GetRedirectInfo());

  if (blocked_reason) {
    resource->FinishAsError(ResourceError::CancelledDueToAccessCheckError(
                                params.Url(), blocked_reason.value()),
                            freezable_task_runner.get());
    return true;
  }
  return false;
}
#endif

#if BUILDFLAG(ARKWEB_EXT_LOG_MESSAGE)
void PrintLoadersToCancel(HeapVector<Member<ResourceLoader>>& loaders_to_cancel,
                          int target) {
  if (loaders_to_cancel.size()) {
#if BUILDFLAG(ARKWEB_LOGGER_REPORT)
    LOG_FEEDBACK(INFO, kNavigation)
        << "StopFetchingResource loadersToCancelSize:"
        << loaders_to_cancel.size() << " target:" << target;
#endif
  }
}
#endif
