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

#if BUILDFLAG(IS_ARKWEB) && BUILDFLAG(ARKWEB_ENABLE_CDM)
#include "media/base/ohos/ohos_media_drm_bridge_client.h"
#endif

#if BUILDFLAG(ARKWEB_WPT)
#include "content/browser/font_unique_name_lookup/font_unique_name_lookup_ohos.h"
#endif  // BUILDFLAG(ARKWEB_WPT)

namespace content {

namespace {

// InProcessDataDecoder will work on IO thread.
class InProcessDataDecoder : public data_decoder::ServiceProvider {
 public:
  InProcessDataDecoder()
      : task_runner_(GetIOThreadTaskRunner({})) {
    data_decoder::ServiceProvider::Set(this);
  }

  InProcessDataDecoder(const InProcessDataDecoder&) = delete;
  InProcessDataDecoder& operator=(const InProcessDataDecoder&) = delete;

  ~InProcessDataDecoder() {
    data_decoder::ServiceProvider::Set(nullptr);
  }

  // ServiceProvider implementation:
  void BindDataDecoderService(
      mojo::PendingReceiver<data_decoder::mojom::DataDecoderService> receiver) {
    if (!task_runner_->RunsTasksInCurrentSequence()) {
      task_runner_->PostTask(
          FROM_HERE,
          base::BindOnce(&InProcessDataDecoder::BindDataDecoderService,
                        weak_ptr_factory_.GetWeakPtr(), std::move(receiver)));
      return;
    }

    receivers_.Add(&service_, std::move(receiver));
  }

 private:
  const scoped_refptr<base::SequencedTaskRunner> task_runner_;
  data_decoder::DataDecoderService service_;
  mojo::ReceiverSet<data_decoder::mojom::DataDecoderService> receivers_;
  base::WeakPtrFactory<InProcessDataDecoder> weak_ptr_factory_{this};
};
}


}  // namespace content