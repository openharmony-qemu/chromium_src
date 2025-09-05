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

static void PushCreateWorkersTids(scoped_refptr<base::internal::WorkerThread>>& worker,
                                  std::vector<int32_t>& create_workers_thread_ids_,
                                  std::vector<scoped_refptr<base::internal::WorkerThread>> remain_workers) {
  auto tid = worker->GetRealTid();
  if (tid) {
    create_workers_thread_ids_.push_back(tid);
  } else {
    remain_workers.push_back(worker);
  }
}

std::vector<int32_t> ReportCreateWorkers() {
  CheckedAutoLock auto_lock(lock_);
  std::vector<int32_t> create_workers_thread_ids_;
  if (create_workers_.size()) {
    std::vector<scoped_refptr<base::internal::WorkerThread>> remain_workers;
    for (auto& worker : create_workers_) {
      if (worker) {
        PushCreateWorkersTids(worker, create_workers_thread_ids_, remain_workers);
      }
    }
    create_workers_.clear();
    if (remain_workers.size()) {
      create_workers_ = std::move(remain_workers);
    }
  }
  return create_workers_thread_ids_;
}

std::vector<int32_t> ReportDestroyWorkers() {
  CheckedAutoLock auto_lock(lock_);
  std::vector<int32_t> destroy_workers_thread_ids_;
  if (destroy_workers_ids_.size()) {
    for (auto& tid : destroy_workers_ids_) {
      destroy_workers_thread_ids_.push_back(tid);
    }
    destroy_workers_ids_.clear();
  }
  return destroy_workers_thread_ids_;
}
