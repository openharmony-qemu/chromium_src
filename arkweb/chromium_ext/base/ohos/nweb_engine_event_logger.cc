/*
 * Copyright (c) 2022-2024 Huawei Device Co., Ltd.
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

#include <queue>
#include <memory>
#include <mutex>
#include <thread>
#include "base/ohos/nweb_engine_event_logger.h"
#include "base/logging.h"
#include "base/task/single_thread_task_runner.h"
#include "base/no_destructor.h"

namespace base {
namespace ohos {

namespace {

struct UploadData {
  std::string module;
  std::string resource;
  std::string error_code;
  std::string error_msg;
};

std::queue<std::shared_ptr<UploadData>>& GetUploadQueue() {
  static NoDestructor<std::queue<std::shared_ptr<UploadData>>> upload_queue;
  return *upload_queue;
}

std::mutex& GetQueueMutex() {
  static NoDestructor<std::mutex> queue_mutex;
  return *queue_mutex;
}

class NWebEngineEventLogger {
 public:
  static NWebEngineEventLogger* Instance() {
    static NoDestructor<NWebEngineEventLogger> instance;
    return instance.get();
  }

  void set_upload_callback(UploadCallbackFunc callback) {
    upload_callback_ = callback;
    task_runner_ = base::SingleThreadTaskRunner::GetCurrentDefault();

    std::lock_guard<std::mutex> lock(GetQueueMutex());
    while (!GetUploadQueue().empty()) {
        std::shared_ptr<UploadData> data = GetUploadQueue().front();
        GetUploadQueue().pop();

        upload_callback(data->module, data->resource, data->error_code, data->error_msg);
    }
  }

  void upload_callback(const std::string& module,
                       const std::string& resource,
                       const std::string& error_code,
                       const std::string& error_msg) {
    if (task_runner_ == nullptr) {
      std::shared_ptr<UploadData> data = std::make_shared<UploadData>();
      data->module = module;
      data->resource = resource;
      data->error_code = error_code;
      data->error_msg = error_msg;

      std::lock_guard<std::mutex> lock(GetQueueMutex());
      GetUploadQueue().push(data);
      return;
    }

    if (!task_runner_->BelongsToCurrentThread()) {
      task_runner_->PostTask(FROM_HERE, base::BindOnce(
                             &NWebEngineEventLogger::upload_callback,
                             base::Unretained(this),
                             module, resource, error_code, error_msg));
      return;
    }
    if (upload_callback_ != nullptr) {
      upload_callback_(module, resource, error_code, error_msg);
    }
  }

 private:
  friend class NoDestructor<NWebEngineEventLogger>;

  NWebEngineEventLogger();
  ~NWebEngineEventLogger() = default;

  UploadCallbackFunc upload_callback_;
  scoped_refptr<SingleThreadTaskRunner> task_runner_;
};

NWebEngineEventLogger::NWebEngineEventLogger() : upload_callback_(nullptr), task_runner_(nullptr) {

}

}  // namespace

BASE_EXPORT void SetUploadCallback(UploadCallbackFunc callback) {
  NWebEngineEventLogger::Instance()->set_upload_callback(callback);
}

BASE_EXPORT void ReportEngineEvent(const std::string& module,
                                   const std::string& resource,
                                   const std::string& error_code,
                                   const std::string& error_msg) {
  NWebEngineEventLogger::Instance()->upload_callback(module, resource, error_code, error_msg);
}

}  // namespace ohos
}  // namespace base