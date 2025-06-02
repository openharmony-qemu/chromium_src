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

#include "event_handler_adapter_impl.h"

#include "arkweb/ohos_nweb/src/nweb_hilog.h"
#include <sys/epoll.h>

namespace OHOS::NWeb {

void FfrtCallback(void* data, uint32_t events)
{
    EventHandlerFDListenerAdapterImpl *fileDescriptorListener = (EventHandlerFDListenerAdapterImpl*)data;
    int32_t fileDescriptor = fileDescriptorListener->GetFileDescriptor();
    auto listener = fileDescriptorListener->GetFDLisener();
    listener->OnReadable(fileDescriptor);
}

EventHandlerFDListenerAdapterImpl::EventHandlerFDListenerAdapterImpl(
    const std::shared_ptr<EventHandlerFDListenerAdapter>& listener, int32_t fileDescriptor)
    : listener_(listener), fileDescriptor_(fileDescriptor)
{}

EventHandlerAdapterImpl::~EventHandlerAdapterImpl()
{
    if (!fdListenerMap_.empty()) {
        for (auto it = fdListenerMap_.begin(); it != fdListenerMap_.end(); ++it) {
            delete it->second;
            it = fdListenerMap_.erase(it);
        }
    }
    if (ffrt_loop_destroy(loop_) != ffrt_success) {
        WVLOG_E("loop destory failed");
    }
}

EventHandlerAdapterImpl::EventHandlerAdapterImpl()
{
    auto mainQueue_ = ffrt_get_main_queue();
    loop_ = ffrt_loop_create(mainQueue_);
    if (loop_ == nullptr) {
        WVLOG_E("create loop failed");
    }
}

bool EventHandlerAdapterImpl::AddFileDescriptorListener(
    int32_t fileDescriptor, uint32_t events, const std::shared_ptr<EventHandlerFDListenerAdapter> listener)
{
    WVLOG_D("[adapter mock] add fileDescriptorListener");
    uint32_t ffrtEvents = events | EPOLLERR | EPOLLHUP;
    EventHandlerFDListenerAdapterImpl *fileDescriptorListener =
        new EventHandlerFDListenerAdapterImpl(listener, fileDescriptor);
    fdListenerMap_[fileDescriptor] = fileDescriptorListener;

    int32_t ret = ffrt_loop_epoll_ctl(loop_, EPOLL_CTL_ADD, fileDescriptor, ffrtEvents,
                                      fileDescriptorListener, FfrtCallback);
    if (ret != ffrt_success) {
        WVLOG_E("add file descriptor listener failed");
    }

    return ret == ffrt_success;
}

void EventHandlerAdapterImpl::RemoveFileDescriptorListener(int32_t fileDescriptor)
{
    WVLOG_D("[adapter mock] remove fileDescriptorListener");
    bool ret = ffrt_loop_epoll_ctl(loop_, EPOLL_CTL_DEL, fileDescriptor, 0, nullptr, nullptr);
    if (ret != ffrt_success) {
        WVLOG_E("remove file descriptor listener failed");
    }

    auto it = fdListenerMap_.find(fileDescriptor);
    if (it != fdListenerMap_.end()) {
        delete it->second;
        fdListenerMap_.erase(it);
    }
}

void EventHandlerAdapterImpl::PostTask(const std::shared_ptr<OnceCallbackAdapter> callback){
}

} // namespace OHOS::NWeb
