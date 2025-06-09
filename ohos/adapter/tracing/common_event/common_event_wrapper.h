/*
 * Copyright (c) 2023-2025 Haitai FangYuan Co., Ltd.
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this list of
 *    conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list
 *    of conditions and the following disclaimer in the documentation and/or other materials
 *    provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors may be used
 *    to endorse or promote products derived from this software without specific prior written
 *    permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef OHOS_ADAPTER_TRACING_COMMON_EVENT_WRAPPER_H_
#define OHOS_ADAPTER_TRACING_COMMON_EVENT_WRAPPER_H_

#include <functional>
#include <memory>
#include <string>
#include <vector>

#include "BasicServicesKit/oh_commonevent.h"

namespace ohos::adapter::common_event {

class SubscribeInfoDeleter;
class EventData;
class SubscriberDeleter;

using SubscribeInfoPtr =
    std::unique_ptr<CommonEvent_SubscribeInfo, SubscribeInfoDeleter>;

class EventData {
 public:
  std::string event;
  std::string data_string;
  std::string bundle_name;
  int code = 0;

  std::string ToString() const;
  static EventData Build(const CommonEvent_RcvData* rcvData);
};

class SubscribeInfoDeleter {
 public:
  void operator()(CommonEvent_SubscribeInfo* info) const;
};

class SubscribeInfo {
 public:
  static SubscribeInfoPtr Build(const std::vector<std::string>& events);
};

class SubscriberDeleter {
 public:
  void operator()(CommonEvent_Subscriber* subscriber) const;
};

class Subscriber {
 public:
  static Subscriber& Instance();

  Subscriber(const Subscriber&) = delete;
  Subscriber& operator=(const Subscriber&) = delete;

  bool Subscribe(const std::vector<std::string>& events);
  void Unsubscribe();
  ~Subscriber();

  void RegisterEventListener(std::function<void(const EventData&)> listener);
  std::function<void(const EventData&)> GetObserver();

  static void OnCommonEvent(const CommonEvent_RcvData* data);

 private:
  Subscriber() = default;
  SubscribeInfoPtr subscriber_info_;
  std::unique_ptr<CommonEvent_Subscriber, SubscriberDeleter> subscriber_;
  std::function<void(const EventData&)> listener_;
};
}  // namespace ohos::adapter::common_event
#endif  // OHOS_ADAPTER_TRACING_COMMON_EVENT_WRAPPER_H_
