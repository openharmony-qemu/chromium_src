// Copyright 2012 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "content/renderer/anr_dumper.h"
#include "base/lazy_instance.h"
#include "base/task/single_thread_task_runner.h"
#include "content/common/child_process.mojom.h"
#include "third_party/blink/renderer/platform/bindings/v8_per_isolate_data.h"
#include "v8/include/v8-extension.h"
#include "v8/include/v8-isolate.h"

namespace {
constexpr int kMaxStackLength = 3000;
}

namespace content {

// static
base::LazyInstance<AnrDumper>::DestructorAtExit g_anr_dumper_instance_ =
    LAZY_INSTANCE_INITIALIZER;

// static
AnrDumper* AnrDumper::GetInstance() {
  return g_anr_dumper_instance_.Pointer();
}

class AnrDumper::InterruptData {
 public:
  InterruptData(
      mojom::ChildProcess::dumpCurrentJavaScriptStackInMainThreadCallback
          callback)
      : callback_(std::move(callback)),
        task_runner_(base::SingleThreadTaskRunner::GetCurrentDefault()) {}

  InterruptData(const InterruptData&) = delete;
  InterruptData& operator=(const InterruptData&) = delete;

  void RunCallback(std::ostringstream stack_trace_stream) {
    const std::string& stack_trace_stream_str = stack_trace_stream.str();
    if (stack_trace_stream_str.length() > kMaxStackLength) {
      task_runner_->PostTask(
          FROM_HERE,
          base::BindOnce(std::move(callback_),
                         stack_trace_stream_str.substr(0, kMaxStackLength)));
      return;
    }
    task_runner_->PostTask(FROM_HERE, base::BindOnce(std::move(callback_),
                                                     stack_trace_stream_str));
  }

 private:
  mojom::ChildProcess::dumpCurrentJavaScriptStackInMainThreadCallback callback_;
  const scoped_refptr<base::SequencedTaskRunner> task_runner_;
};

// Run in io thread, make sure MainThreadIsolate is inited.
void AnrDumper::DumpCurrentJavaScriptStack(
    mojom::ChildProcess::dumpCurrentJavaScriptStackInMainThreadCallback
        callback) {
  InterruptData* interrupt_data = new InterruptData(std::move(callback));
  v8::Isolate* isolate = blink::V8PerIsolateData::MainThreadIsolate();
  isolate->RequestInterrupt(
      [](v8::Isolate* isolate, void* data) {
        std::ostringstream stack_trace_stream;

        v8::Message::PrintCurrentStackTrace(isolate, stack_trace_stream);
        InterruptData* interrupt_data = static_cast<InterruptData*>(data);
        interrupt_data->RunCallback(std::move(stack_trace_stream));
        delete interrupt_data;
      },
      interrupt_data);
}

}  // namespace content
