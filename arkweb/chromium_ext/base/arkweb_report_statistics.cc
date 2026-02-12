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

#include <thread>

#include "base/arkweb_report_statistics.h"
#include "base/logging.h"
#include "base/task/thread_pool.h"
#include "base/task/single_thread_task_runner.h"
#include "base/no_destructor.h"

namespace base {
namespace ohos {

namespace {

class ReportStatistic {
  public:
  static ReportStatistic* GetInstance(){
    static NoDestructor<ReportStatistic> instance;
    return instance.get();
  }

  NO_SANITIZE("cfi-icall")
  void ReportStatisticLog(const std::string& content) {
    if (task_runner_ == nullptr) {
      return;
    }
    if (!task_runner_->BelongsToCurrentThread()) {
      task_runner_->PostTask(FROM_HERE, base::BindOnce(
                            &ReportStatistic::ReportStatisticLog,
                            base::Unretained(this),
                            content));
      return;
    } else if (on_report_statistic_log_callback_) {
        on_report_statistic_log_callback_(content.c_str());
    }
  }

  void SetOnReportStatisticLogCallback(OnReportStatisticLogFunc func) {
    on_report_statistic_log_callback_ = func;
  }

  void SetReportStatisticTaskRunner() {
    task_runner_ = base::SingleThreadTaskRunner::GetCurrentDefault();
  }
  private:
    friend class NoDestructor<ReportStatistic>;
    
    ReportStatistic();
    ~ReportStatistic() = default;

    OnReportStatisticLogFunc on_report_statistic_log_callback_;
    scoped_refptr<SingleThreadTaskRunner> task_runner_;
};

ReportStatistic::ReportStatistic() : on_report_statistic_log_callback_(nullptr), task_runner_(nullptr) {
}

}  // namespace

BASE_EXPORT void SetOnReportStatisticLogCallback(OnReportStatisticLogFunc func){
  ReportStatistic::GetInstance()->SetOnReportStatisticLogCallback(func);
}

BASE_EXPORT void ReportStatisticLog(const std::string& content){
  ReportStatistic::GetInstance()->ReportStatisticLog(content);
}

BASE_EXPORT void SetReportStatisticTaskRunner(){
  ReportStatistic::GetInstance()->SetReportStatisticTaskRunner();
}

}  // namespace ohos
}  // namespace base
