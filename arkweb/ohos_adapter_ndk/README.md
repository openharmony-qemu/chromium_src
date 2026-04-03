# arkweb/ohos_adapter_ndk

## Overview

The `ohos_adapter_ndk` directory contains the OpenHarmony NDK (Native Development Kit) adapter layer. This module is responsible for adapting Chromium's underlying calls to OpenHarmony system NDK interfaces, achieving cross-platform compatibility.

## Directory Structure

```
ohos_adapter_ndk/
├── BUILD.gn                        # GN build configuration
├── README.md                       # This document
├── ndk_version.*                   # NDK version management
├── ohos_adapter_helper_decouple.*  # Adapter helper decoupling
├── ndk_tools/                      # NDK tools
│   ├── adapter_ndk_api.json        # NDK API configuration
│   └── gen_ndk.py                  # NDK generation script
├── interfaces/                     # Interface definitions
├── stub/                          # Stub implementation
└── [various adapter directories]    # Adapters categorized by function
```

## Adapter Categories

### 1. System Capability Adapters

#### ability_runtime_adapter/
Ability runtime adapter, handling application lifecycle and Ability-related functionality.

- `native_ability_runtime_adapter_impl.*`: Native Ability runtime implementation

#### access_token_adapter/
Access token adapter, handling permissions and access control.

- `access_token_adapter_impl.*`: Access token implementation

#### event_handler_adapter/
Event handling adapter, handling system event dispatch.

- `event_handler_adapter/`: Event handling related

### 2. Multimedia Adapters

#### media_adapter/
Media codec adapter.

- `capability_data_adapter_impl.*`: Capability data adapter
- `media_codec_list_adapter_impl.*`: Codec list
- `drm_adapter_impl.*`: DRM adapter
- `media_codec_decoder_adapter_impl.*`: Decoder adapter
- `audio_codec_decoder_adapter_impl.*`: Audio decoder
- `video_encoder_adapter_impl.*`: Video encoder
- `buffer_info_adapter_impl.*`: Buffer info
- `codec_format_adapter_impl.*`: Codec format

#### audio_capturer_adapter/
Audio capture adapter.

- `audio_capturer_adapter_impl.*`: Audio capture implementation

#### screen_capture_adapter/
Screen capture adapter.

- `screen_capture_adapter_impl.*`: Screen capture implementation

#### media_avsession_adapter/
Media AVSession adapter.

- `media_avsession_adapter_impl.*`: AVSession implementation

### 3. Graphics and Display Adapters

#### graphic_adapter/
Graphics adapter, handling graphics rendering related functionality.

- `native_window_adapter_impl.*`: Native window
- `native_image_adapter_impl.*`: Native image
- `window_adapter_impl.*`: Window management
- `vsync_adapter_impl.*`: Vertical sync
- `ashmem_adapter_impl.*`: Ashmem shared memory

#### display_manager_adapter/
Display manager adapter.

- `native_display_manager_adapter_impl.*`: Display management implementation

#### window_manager_adapter/
Window manager adapter.

- `window_manager_adapter_impl.*`: Window management implementation

#### ohos_image_adapter/
Image processing adapter.

- `ohos_image_decoder_adapter_impl.*`: Image decoding
- `ohos_image_encoder_adapter.*`: Image encoding

#### ohos_native_buffer_adapter/
Native buffer adapter.

- `ohos_native_buffer_adapter_impl.*`: Native buffer implementation

#### drawing_text_adapter/
Text drawing adapter.

- `ohos_drawing_text_adapter_impl.*`: Text drawing implementation

### 4. Network Adapters

#### net_connect_adapter/
Network connection adapter.

- `net_connect_adapter_impl.*`: Network connection
- `net_connect_utils.*`: Network utilities
- `net_connection_properties_adapter_impl.*`: Connection properties
- `net_capabilities_adapter_impl.*`: Network capabilities

#### net_config_adapter/
Network configuration adapter.

- `net_config_adapter_impl.*`: Network configuration

#### net_event_adapter/
Network event adapter.

- `net_event_adapter_impl.*`: Network events

#### net_proxy_adapter_impl/
Network proxy adapter.

- `net_proxy_adapter_impl.*`: Proxy implementation

### 5. Input Adapters

#### inputmethodframework_adapter/
Input method framework adapter.

- `imf_adapter_impl.*`: IMF implementation
- `third_party/cJSON/`: JSON parsing library

#### multimodalinputnew_adapter/
Multimodal input adapter.

- `mmi_new_adapter_impl.*`: MMI implementation

### 6. Storage Adapters

#### datashare_adapter/
Data sharing adapter.

- `datashare_adapter_impl.*`: Data sharing implementation

#### distributeddatamgr_adapter/
Distributed data management adapter.

- `ohos_web_snapshot_data_base.*`: Web snapshot database

#### keystore_adapter/
Keystore adapter.

- `keystore_adapter_impl.*`: Keystore implementation

### 7. Clipboard Adapter

#### pasteboard_adapter/
Clipboard adapter.

- `src/pasteboard_client_adapter_impl.*`: Clipboard client
- `src/pasteboard_client_adapter_utils.*`: Clipboard utilities

### 8. Sensor Adapters

#### sensor_adapter/
Sensor adapter.

- `sensor_adapter_impl.*`: Sensor implementation

#### vibrator_adapter/
Vibrator adapter.

- `vibrator_adapter_impl.*`: Vibrator implementation

#### battery_mgr_adapter/
Battery manager adapter.

- `battery_mgr_client_adapter_impl.*`: Battery management client

### 9. Location Adapter

#### location_adapter/
Location adapter.

- `location_adapter/`: Location related

### 10. Security Adapters

#### cert_mgr_adapter/
Certificate manager adapter.

- `cert_mgr_adapter_impl.*`: Certificate management implementation

#### security_adapter/
Security adapter.

- `arkts_security_adapter_impl.*`: ArkTS security adapter

### 11. System Service Adapters

#### system_properties_adapter/
System properties adapter.

- `system_properties_adapter/`: System properties

#### hiviewdfx_adapter/
HiView DFx adapter.

- `hilog_adapter.*`: Log adapter
- `hitrace_adapter_impl.*`: Trace adapter
- `hiappevent_adapter_impl.*`: App event adapter

#### color_picker_adapter/
Color picker adapter.

- `color_picker_adapter_impl.*`: Color picker implementation

### 12. Time and Formatting Adapters

#### date_time_format_adapter/
Date/time formatting adapter.

- `date_time_format_adapter_impl.*`: Date/time formatting

### 13. Print Adapter

#### print_manager_adapter/
Print manager adapter.

- `print_manager_adapter/`: Print management

### 14. ArkTS Adapter

#### arkts_adapter/
ArkTS runtime adapter.

- `arkts_hilog_adapter.*`: ArkTS log adapter

### 15. Utilities and Helpers

#### utils/
Utility functions.

- `src/ashmem.*`: Ashmem utilities
- Other common utilities

#### ndk_callback_wrapper/
NDK callback wrapper.

- `callback_shared_wrapper.h`: Callback shared wrapper

#### ndk_tools/
NDK tool scripts.

- `gen_ndk.py`: NDK code generation
- `adapter_ndk_api.json`: API configuration
- `check_ndk.py`: NDK check script

### 16. Mock Implementation

#### mock_ndk_api/
Mock NDK API for testing.

- `mock_base_ohos/`: Base mock implementation

## Core Components

### NDK Version Management
- `ndk_version.cc/h`: NDK version information
- `ndk_version_unittest.cc`: Version unit tests

### Adapter Helper
- `ohos_adapter_helper_decouple.cpp`: Adapter decoupling implementation
- `ohos_adapter_helper_ext.h`: Extended helper interface
- `ohos_adapter_helper_decouple_unittest.cpp`: Unit tests

### Interface Definitions
- `interfaces/ohos_adapter_helper.h`: Adapter helper interface
- `interfaces/ark_ohos_adapter_helper_wrapper.h`: Wrapper interface
- Other module interface definitions

## Build System

### GN Build Targets

```gn
# NDK generator
action("adapter_ndk_gen")

# Stub library
shared_library("adapter_ndk_stub")

# Wrapper library
source_set("adapter_ndk_wrapper")

# Main component
component("adapter_ndk")
```

### NDK API Generation

Use the `gen_ndk.py` script to automatically generate NDK binding code based on `adapter_ndk_api.json`:

```bash
python gen_ndk.py \
    --input adapter_ndk_api.json \
    --output <target_dir> \
    --version 15
```

## Configuration Options

### BUILD.gn Parameters

```gn
declare_args() {
  # WebView only mode
  webview_only = true

  # Enable printing
  webview_print_enable = true

  # Use fuzzing engine
  use_fuzzing_engine = false
}
```

## Platform Support

Supports the following CPU architectures:
- `arm`: ARM 32-bit
- `arm64`: ARM 64-bit
- `x86_64`: Intel 64-bit

Each architecture links to the corresponding OpenHarmony NDK libraries.

## Dependencies

### System Library Dependencies
- `libhilog_ndk.z.so`: Logging library
- `libnative_window.so`: Native window
- `libnative_buffer.so`: Native buffer
- `libnative_image.so`: Native image
- `libohaudio.so`: Audio library
- `libnative_media_codecbase.so`: Media codec
- And other OpenHarmony system libraries...

### Internal Dependencies
- `//arkweb/glue`: Glue layer
- `//third_party:huawei_securec`: Secure C library
- `//third_party/boringssl`: Crypto library
- `//third_party/skia`: Graphics library
- `//third_party/jsoncpp`: JSON library

## Usage Examples

### Using Adapters

```cpp
#include "ohos_adapter_helper.h"

// Get adapter instance
auto* adapter = OHOSAdapterHelper::GetInstance();

// Call adapter method
adapter->SomeMethod();
```

### Adding New Adapters

1. Create adapter implementation files in the corresponding directory
2. Define interfaces in `interfaces/`
3. Add API definitions in `ndk_tools/adapter_ndk_api.json`
4. Run `gen_ndk.py` to generate binding code
5. Add source files to `BUILD.gn`

## Testing

Unit tests:
```bash
ninja -C out/Default adapter_ndk_unittests
```

Mock API for testing:
```bash
ninja -C out/Default mock_base_ohos
```

## Notes

1. **API Version**: Currently using NDK API Version 15
2. **Library Linking**: Must link to system libraries of corresponding architecture
3. **Conditional Compilation**: Control features via `webview_only` and `webview_print_enable`
4. **Decoupling Design**: Adapters reduce direct dependency on system APIs through decoupling mechanism

## Maintainers

See CODEOWNERS file

## Related Documentation
- [OpenHarmony NDK Documentation](https://docs.openharmony.cn/)
- [NDK API Reference](https://docs.openharmony.cn/application-dev/developer-libs/native-libs/)
