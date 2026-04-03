# arkweb/ohos_nweb

## Overview

This directory provides a wrapper layer for the underlying web kernel on the OHOS system, implementing a Chromium-based web rendering engine. It offers complete web content loading, rendering, and interaction capabilities for OpenHarmony applications.

## Directory Structure

```
ohos_nweb/
├── BUILD.gn                    # GN build configuration
├── README.md                   # This document
├── browser/                    # Browser-side implementation
├── include/                    # Interfaces and definitions exposed to OHOS
├── src/                        # Wrapper implementation for web kernel
│   ├── cef_delegate/           # CEF interface wrapper
│   ├── capi/                   # C API interface
│   ├── ndk/                    # NDK interface implementation
│   ├── sysevent/               # System event reporting
│   ├── browser/                # Browser-related (performance management)
│   └── download/               # Download management
└── test/                       # Test code
```

### Subdirectory Descriptions

- **include/**: Provides interfaces and definitions exposed to OHOS. OHOS side can load and use the web kernel through these methods.
  - `nweb.h`: Core interface definitions
  - `nweb_engine.h`: Engine initialization interface
  - `nweb_preference_delegate.h`: Configuration delegate
  - `nweb_handler.h`: Event handling interface
  - Other functional module interfaces

- **src/**: Wrapper implementation for the web kernel, where subdirectories represent wrappers for different web kernels, and other parts contain common logic implementations

  - **cef_delegate/**: CEF interface wrapper, including all event delegate implementations
    - `nweb_delegate.*`: Main delegate class
    - `nweb_event_handler.*`: Event handling
    - `nweb_render_handler.*`: Rendering handling
    - `nweb_resource_handler.*`: Resource loading
    - `nweb_find_delegate.*`: Find functionality
    - Extension-related delegates (bookmarks, history, downloads, tabs, etc.)

  - **capi/**: C API interface implementation
    - `browser_service/`: Browser service interface
    - Extension callback interface definitions
    - DevTools message handling

  - **ndk/**: NDK interface implementation
    - `arkweb_native_object.*`: Native object
    - `arkweb_native_javascript_execute_callback.*`: JS execution callback
    - `arkweb_native_web_message_callback.*`: Web message callback
    - `scheme_handler/`: Custom protocol handling

  - **sysevent/**: System event reporting
    - `event_reporter.*`: Event reporter
    - `oh_web_performance_timing.*`: Performance timing collection

  - **browser/**: Browser-side implementation
    - `performance_manager/`: Performance management
      - `mechanisms/`: Mechanism implementation
      - `policies/`: Policy implementation (BFCache, background tasks, etc.)

  - **download/**: Download management
    - `web_downloader.*`: Web downloader

## Core Functional Modules

### 1. NWEB Engine (nweb_engine)

The NWEB engine is the core of the entire module, providing web content loading and rendering capabilities.

**Main Features**:
- Page loading and navigation
- JavaScript execution
- Cookie management
- Web Storage support
- Network request interception
- Download management
- Find functionality
- Zoom and text selection
- Full-screen support

### 2. Extension Feature Support

Conditionally compiled extension features:
- 🔧 `arkweb_arkweb_extensions`: Extension system support
- 🔧 `arkweb_drag_resize`: Drag-to-resize
- 🔧 `arkweb_custom_video_player`: Custom video player
- 🔧 `arkweb_ext_navigation`: Enhanced navigation
- 🔧 `arkweb_performance_persistent_task`: Performance persistent tasks
- 🔧 `arkweb_bfcache`: BFCache support
- 🔧 `arkweb_notification`: Notification support
- 🔧 `arkweb_scheme_handler`: Scheme Handler
- 🔧 `arkweb_html_select`: HTML selector
- 🔧 `arkweb_drag_drop`: Drag and drop support
- 🔧 `arkweb_multi_window`: Multi-window
- 🔧 `arkweb_ai`: AI features
- 🔧 `arkweb_autolayout`: Auto layout
- 🔧 `arkweb_video_assistant`: Video assistant

### 3. Performance Features

- First Meaningful Paint (FMP) details
- Largest Contentful Paint (LCP) details
- Load committed details
- Frame loss reporting

## Dependencies

### Internal Dependencies
- `//cef`: CEF framework
- `//ohos_glue`: OpenHarmony glue layer
- `//arkweb/build/features`: Feature configuration
- `//arkweb/ohos_adapter_ndk`: NDK adapter

### External Dependencies
- `//base`: Chromium base library
- `//content/public/common`: Content public interfaces
- `//components/js_injection`: JS injection component
- `//third_party/boringssl`: Crypto library
- `//third_party/icu`: Internationalization component

## Building

Use the build.sh script in the nweb root directory to build with default parameters, outputting to the default `out/Default` path. You can also customize the build:

```bash
gn args <output_path> # Customize build parameters and output path
autoninja -C <output_path> libarkweb_engine libarkweb_renderer
```

The nweb side will compile:
- `libarkweb_engine.so`, containing the entire chromium kernel and cef framework implementation
- `libarkweb_renderer`, containing the render child process startup implementation

Both files are located in the `<output_path>` directory.

Additionally, if code was pulled via web_all_build.xml, you can use the build.sh script in the parent directory of the nweb root for a complete build, which will first compile the nweb libraries and automatically copy the libraries required by the OHOS side to the designated location.

## On-Device Running

Refer to the OHOS side README instructions

## Testing

Unit tests are located in the `test/` directory, covering the following modules:
- CEF delegate layer tests
- NDK interface tests
- Download management tests
- Performance management tests
- Extension feature tests

Run tests:
```bash
ninja -C out/Default arkweb_test_targets
```

## Notes

1. **Platform Support**: Primarily for OpenHarmony platform
2. **Architecture Support**: ARM, ARM64, x86_64
3. **Feature Toggles**: Control feature enablement via GN args
4. **NDK Version**: Must match OpenHarmony NDK version

## Maintainers

See CODEOWNERS file

## Related Documentation
- [ArkWeb Architecture Documentation](../README.md)
- [CEF Documentation](https://bitbucket.org/chromiumembedded/cef/wiki)
