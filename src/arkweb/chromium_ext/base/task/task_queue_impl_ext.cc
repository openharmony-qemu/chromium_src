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

#include <sstream>
#include <cstdlib>
#include <string>

#include "base/logging.h"
#include "task_queue_impl_ext.h"

extern "C" __attribute__((weak) )void set_fatal_message(const char *msg);

#if BUILDFLAG(ARKWEB_CRASHPAD)
namespace base {

namespace sequence_manager {

namespace internal {

void TaskCheckWithFatalMessage(const Task& task) {
    if(!task.task) {
        const char* task_function_name = task.posted_from.function_name();
        const char* task_file_name = task.posted_from.file_name();
        int task_line_number = task.posted_from.line_number();

        std::stringstream ss;
        ss << "task function_name: " << task_function_name
           << " task file_name: " << task_file_name
           << " task line_number: " << task_line_number;
        std::string str_result = ss.str();
        const char* result = str_result.c_str();

        LOG(WARNING) << str_result;
        set_fatal_message(result);
        abort();
    }
}

} // namespace internal

} // namespace sequence_manager

} // namespace base
#endif
