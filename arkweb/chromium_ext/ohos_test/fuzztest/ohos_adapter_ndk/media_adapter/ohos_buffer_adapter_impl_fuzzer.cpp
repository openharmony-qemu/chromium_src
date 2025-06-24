#include <fuzzer/FuzzedDataProvider.h>
#include <cstdint>
#include <cstdlib>

#include "ohos_buffer_adapter_impl.h"

using namespace OHOS::NWeb;

namespace OHOS {
constexpr int MAX_SET_NUMBER = 1000;

bool OhosBufferAdapterImplFuzzTest(const uint8_t* data, size_t size)
{
    FuzzedDataProvider fdp(data, size);

    OhosBufferAdapterImpl bufferAdapter;

    uint8_t randomAddr = fdp.ConsumeIntegral<uint8_t>();
    uint32_t randomSize = fdp.ConsumeIntegral<uint32_t>();

    bufferAdapter.SetAddr(&randomAddr);
    bufferAdapter.SetBufferSize(randomSize);

    bufferAdapter.GetAddr();
    bufferAdapter.GetBufferSize();

    return true;
}
} // namespace OHOS


extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size) {
    OHOS::OhosBufferAdapterImplFuzzTest(data, size);
    return 0;
}