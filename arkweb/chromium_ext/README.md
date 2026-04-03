# arkweb/chromium_ext

## Overview

The `chromium_ext` directory contains extensions and modifications to the Chromium kernel. This module adapts Chromium to the OpenHarmony platform through extensions, patches, and new features, adding specific functional enhancements.

## Directory Structure

```
chromium_ext/
├── BUILD.gn                        # GN build configuration
├── chromium_ext.gni                # Chromium extension configuration
├── README.md                       # This document
├── base/                           # Base library extensions
├── blink/                          # Blink rendering engine extensions
├── cc/                             # Compositor extensions
├── cef/                            # CEF framework extensions
├── chrome/                         # Chrome browser extensions
├── components/                     # Component extensions
├── content/                        # Content module extensions
├── device/                         # Device access extensions
├── extensions/                     # Extension system
├── gpu/                            # GPU extensions
├── ipc/                            # IPC communication extensions
├── media/                          # Media framework extensions
├── mojo/                           # Mojo communication extensions
├── net/                            # Network stack extensions
├── pdf/                            # PDF rendering extensions
├── sandbox/                        # Sandbox mechanism extensions
├── services/                       # Service layer extensions
├── servieces/                      # Typo directory
├── service/                        # Single service extension
├── skia/                           # Skia graphics library extensions
├── storage/                        # Storage extensions
├── third_party/                    # Third-party library extensions
├── ui/                             # UI framework extensions
├── url/                            # URL handling extensions
├── v8/                             # V8 engine extensions
└── ohos_test/                      # OpenHarmony tests
```

## Main Extension Modules

### 1. base/ - Base Library Extensions

OpenHarmony adaptation and extensions for the base library.

**Main Files**:
- `arkweb_base_ext.gni`: Base extension configuration
- `base_paths_ohos.*`: OpenHarmony path definitions
- `feature_list_utils.*`: Feature list utilities
- `logging_for_include.*`: Logging functionality
- `datashare_uri_utils.*`: Data share URI utilities
- `report_loss_frame_ext.*`: Frame loss reporting
- `base_switches_for_include.*`: Switch definitions

**Subdirectories**:
- `allocator/`: Memory allocator
- `debug/`: Debug functionality
- `files/`: File operations
- `i18n/`: Internationalization
- `memory/`: Memory management
- `message_loop/`: Message loop
- `metrics/`: Metrics collection
- `ohos/`: OpenHarmony-specific implementations
- `power_monitor/`: Power monitoring
- `process/`: Process management
- `task/`: Task scheduling
- `test/`: Test support
- `time/`: Time handling
- `trace_event/`: Trace events

### 2. content/ - Content Module Extensions

Core extensions to the Chromium content layer.

**Subdirectories**:
- `browser/`: Browser process extensions
  - WebContents related
  - Navigation control
  - Download management
- `child/`: Child process extensions
- `common/`: Common interfaces
- `gpu/`: GPU process
- `public/`: Public APIs
- `renderer/`: Renderer process
- `shell/`: Shell integration

### 3. components/ - Component Extensions

Extensions for various Chromium components.

**Main Components**:
- `autofill/`: Autofill
- `captcha_portal/`: CAPTCHA portal
- `cdm/`: Content decryption
- `content_settings/`: Content settings
- `crash/`: Crash handling
- `download/`: Download management
- `embedder_support/`: Embedder support
- `js_injection/`: JavaScript injection
- `keyed_service/`: Keyed service
- `os_crypt/`: OS encryption
- `password_manager/`: Password management
- `pdf/`: PDF support
- `permissions/`: Permission management
- `prefs/`: Preferences
- `viz/`: Visualization compositing

### 4. ui/ - UI Framework Extensions

OpenHarmony adaptation for the UI framework.

**Subdirectories**:
- `base/`: UI base
- `compositor/`: Compositor
- `events/`: Event handling
- `gl/`: OpenGL integration
- `native_theme/`: Native theme
- `ohos/`: OpenHarmony UI
- `ozone/`: Ozone display backend
- `shell_dialogs/`: Shell dialogs
- `touch_selection/`: Touch selection
- `views/`: Views system
- `webui/`: Web UI

### 5. net/ - Network Stack Extensions

OpenHarmony adaptation for the networking layer.

**Subdirectories**:
- `base/`: Network base
- `cert/`: Certificate handling
- `dns/`: DNS resolution
- `http/`: HTTP protocol
- `nqe/`: Network quality estimation
- `proxy_resolution/`: Proxy resolution
- `quiche/`: QUIC protocol
- `shared_dictionary/`: Shared dictionary
- `socket/`: Socket interface
- `ssl/`: SSL/TLS
- `url_request/`: URL request
- `websockets/`: WebSocket

**Configuration**:
```gn
config("net_dns_config") {
  ldflags = [
    "-Wl,--wrap=GetOHFreeDnsResult",
    "-Wl,--wrap=GetOHGetAddrInfoForNetwork"
  ]
}
```

### 6. media/ - Media Framework Extensions

OpenHarmony adaptation for multimedia processing.

**Subdirectories**:
- `audio/`: Audio processing
- `base/`: Media base
- `capture/`: Capture devices
- `cdm/`: Content decryption module
- `ffmpeg/`: FFmpeg integration
- `filters/`: Media filters
- `gpu/`: GPU acceleration
- `mojo/`: Mojo interface
- `renderers/`: Renderers
- `video/`: Video processing

### 7. device/ - Device Access Extensions

Extensions for device access functionality.

**Subdirectories**:
- `battery/`: Battery status
- `bluetooth/`: Bluetooth
- `gamepad/`: Gamepad
- `vr/`: Virtual reality

### 8. gpu/ - GPU Extensions

GPU rendering related extensions.

**Main Functions**:
- Command buffers
- Shader processing
- Texture management
- EGL/GLES integration

### 9. services/ - Service Layer Extensions

Extensions to the Chromium service layer.

**Main Services**:
- Network service
- Media service
- Device service
- Storage service

### 10. Other Extension Modules

#### chrome/
Chrome browser specific extensions.

#### extensions/
Extension system support.

#### ipc/
Inter-process communication extensions.

#### mojo/
Mojo IPC system extensions.

#### pdf/
PDF rendering engine extensions.

#### sandbox/
Sandbox security mechanism extensions.

#### skia/
Skia graphics library extensions.

#### storage/
Storage system extensions.

#### third_party/
Third-party library OpenHarmony adaptations.

#### url/
URL parsing and handling extensions.

#### v8/
V8 JavaScript engine extensions.

### 11. ohos_test/
OpenHarmony specific test code.

## Configuration Files

### chromium_ext.gni

Main configuration file defining compilation options for all extension modules.

**Main Configuration**:
```gni
# Enable/disable extension modules
# Define OpenHarmony specific features
# Configure compilation options
```

## Build Targets

### BUILD.gn

```gn
# Network configuration
config("net_dns_config")

# Device unit test configuration
config("arkweb_device_unittest_config")

# NWEB unit test configuration
config("ohos_nweb_unittests_config")
```

## Main Features

### 1. OpenHarmony System Integration

- File system path adaptation
- System service calls
- Native UI integration
- Device capability access

### 2. Performance Optimization

- Frame loss monitoring and reporting
- Memory management optimization
- Rendering performance improvements
- Resource loading optimization

### 3. Security Enhancements

- Sandbox mechanism adaptation
- Permission management
- Data encryption
- Network security

### 4. Feature Extensions

- Custom protocol handling
- JavaScript injection
- Extension system support
- DevTools integration

## Dependencies

### Internal Dependencies
- `//arkweb/build`: Build system
- `//arkweb/ohos_adapter_ndk`: NDK adapter
- `//arkweb/ohos_nweb`: NWEB module

### External Dependencies
- Chromium source code
- OpenHarmony SDK
- Third-party libraries

## Development Guide

### Adding New Extensions

1. Create extension files in the corresponding module directory
2. Add compilation target in `BUILD.gn`
3. Add configuration in `chromium_ext.gni`
4. Write unit tests
5. Update documentation

### Code Standards

- Follow Chromium code style
- Add OpenHarmony specific comments
- Use namespaces for isolation
- Provide unit tests

## Testing

Each extension module should include corresponding unit tests.

Run tests:
```bash
ninja -C out/Default chromium_ext_unittests
```

## Notes

1. **Version Synchronization**: Extension code must stay synchronized with Chromium version
2. **API Changes**: Be aware of Chromium API changes
3. **Performance Impact**: Extension code should not significantly impact performance
4. **Compatibility**: Ensure compatibility across OpenHarmony versions
5. **Security Review**: All extensions require security review

## Maintainers

See CODEOWNERS file

## Related Documentation
- [Chromium Source Documentation](https://www.chromium.org/developers)
- [OpenHarmony Documentation](https://docs.openharmony.cn/)
