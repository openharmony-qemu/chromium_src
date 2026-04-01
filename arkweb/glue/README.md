# arkweb/glue

## Overview

The `glue` directory contains the glue layer code for ArkWeb. This directory is copied from `src/../deps_code/webview` and defines the interface between ArkUI's ArkWeb Component and ArkWebCore.

**For more details, refer to**: `deps_code/webview/prepare.sh`

## Directory Structure

```
glue/
├── README.md                       # This document
├── CODEOWNERS                      # Code owners
└── [glue layer implementation files]
```

## Design Goals

### 1. Interface Definition

Defines communication interfaces between ArkUI components and ArkWebCore:

- Component lifecycle management
- Event notification mechanism
- Data passing interface
- State synchronization

### 2. Bridging Role

Connects upper-layer ArkUI and lower-level Web engine:

```
┌─────────────────────────────────────────────────────┐
│                  ArkUI Application                   │
└────────────────────────┬────────────────────────────┘
                         │
                         ▼
┌─────────────────────────────────────────────────────┐
│              ArkWeb Component (ArkTS)               │
└────────────────────────┬────────────────────────────┘
                         │
                         ▼
┌─────────────────────────────────────────────────────┐
│                  Glue Layer (C/C++)                 │
└────────────────────────┬────────────────────────────┘
                         │
                         ▼
┌─────────────────────────────────────────────────────┐
│                  ArkWebCore (Chromium)              │
└─────────────────────────────────────────────────────┘
```

## Core Features

### 1. Component Lifecycle

Manages ArkWeb component lifecycle:

```cpp
// Create component
OHOS_NWeb_Create(nweb_id);

// Destroy component
OHOS_NWeb_Destroy(nweb_id);
```

### 2. Page Loading

Controls page loading flow:

```cpp
// Load URL
OHOS_NWeb_LoadUrl(nweb_id, url);

// Load HTML data
OHOS_NWeb_LoadData(nweb_id, data, mime_type);

// Forward/back
OHOS_NWeb_GoBack(nweb_id);
OHOS_NWeb_GoForward(nweb_id);
```

### 3. JavaScript Interaction

Implements JavaScript and native code interaction:

```cpp
// Execute JavaScript
OHOS_NWeb_ExecuteJavaScript(nweb_id, script, callback);

// Register JavaScript object
OHOS_NWeb_AddJavaScriptObject(nweb_id, object_name, object);
```

### 4. Event Notification

Notifies ArkUI of Web engine events:

```cpp
// Page load started
OnLoadStarted(nweb_id, url);

// Page load finished
OnLoadFinished(nweb_id, url);

// Render complete
OnRenderProcessReady(nweb_id);
```

## Interface Categories

### Component Management Interfaces

- Create/destroy components
- Component configuration
- Status queries

### Page Navigation Interfaces

- URL loading
- History navigation
- Refresh/stop

### JavaScript Interfaces

- Script execution
- Object injection
- Message communication

### Event Callback Interfaces

- Load events
- Render events
- Error events
- Input events

### Configuration Interfaces

- User agent settings
- Cache configuration
- Cookie management
- Permission settings

## Usage Examples

### ArkTS Side Usage

```typescript
// ArkTS code
import webview from '@ohos.web.webview';

@Entry
@Component
struct WebPage {
  controller: webview.WebviewController = new webview.WebviewController();

  build() {
    Column() {
      Web({ src: "https://www.example.com" })
        .onPageBegin((event) => {
          console.info("Page start: " + event.url);
        })
        .onPageEnd((event) => {
          console.info("Page finish: " + event.url);
        })
    }
  }
}
```

### C++ Side Implementation

```cpp
// C++ Glue layer implementation
void OHOS_NWeb_LoadUrl(int nweb_id, const std::string& url) {
    auto nweb = GetNWebById(nweb_id);
    if (nweb) {
        nweb->LoadUrl(GURL(url));
    }
}

void OnLoadStarted(int nweb_id, const GURL& url) {
    // Notify ArkUI through Glue layer
    SendEventToArkUI(nweb_id, "onPageBegin", url.spec());
}
```

## Relationships with Other Modules

### With ohos_nweb

- `glue`: Defines interface and bridging logic
- `ohos_nweb`: Implements Web engine core functionality

### With ohos_adapter_ndk

- `glue`: Calls adapter interfaces
- `ohos_adapter_ndk`: Provides system API adaptation

### With chromium_ext

- `glue`: Upper glue layer
- `chromium_ext`: Lower-level Chromium extensions

## Build Configuration

### BUILD.gn

```gn
# Glue layer component
component("ohos_adapter_glue_source") {
  sources = [
    # Glue layer source files
  ]

  public_deps = [
    "//arkweb/ohos_adapter_ndk",
    "//arkweb/ohos_nweb",
  ]

  include_dirs = [
    "//arkweb/glue",
    "//ohos_sdk/.../arkui",
  ]
}
```

## Data Flow

### ArkUI → WebCore

```
ArkUI (ArkTS)
    ↓ (N-API call)
Glue Layer (C++)
    ↓ (Function call)
NWEB (C++)
    ↓ (Method call)
Chromium Content
```

### WebCore → ArkUI

```
Chromium Content
    ↓ (Event callback)
NWEB (C++)
    ↓ (Event forward)
Glue Layer (C++)
    ↓ (N-API callback)
ArkUI (ArkTS)
```

## Debugging

### Enable Logging

```cpp
// Enable Glue layer logging
#define GLUE_LOG_VERBOSE
```

### Trace Calls

```cpp
// Add trace point
TRACE_EVENT("glue", "OHOS_NWeb_LoadUrl", "url", url.c_str());
```

## Best Practices

### 1. Thread Safety

Glue layer code must be thread-safe:

```cpp
// Use thread lock
base::Lock nweb_map_lock_;

// Safe access
{
    base::AutoLock lock(nweb_map_lock_);
    auto nweb = nweb_map_[nweb_id];
}
```

### 2. Error Handling

```cpp
// Return error code
int OHOS_NWeb_LoadUrl(int nweb_id, const std::string& url) {
    auto nweb = GetNWebById(nweb_id);
    if (!nweb) {
        return ERROR_INVALID_NWEB_ID;
    }
    // ...
    return SUCCESS;
}
```

### 3. Memory Management

```cpp
// Use smart pointers for lifecycle management
std::unique_ptr<NWeb> CreateNWeb() {
    return std::make_unique<NWebImpl>();
}
```

## Known Limitations

1. **Performance Overhead**: Cross-language calls have some performance overhead
2. **Type Conversion**: Types need to be converted between ArkTS and C++
3. **Async Handling**: Some operations require async handling

## Future Plans

- [ ] Optimize cross-language call performance
- [ ] Support more data types
- [ ] Enhance error handling
- [ ] Improve debugging tools

## Maintainers

See CODEOWNERS file

## Related Documentation
- [ArkUI Documentation](https://docs.openharmony.cn/ui)
- [N-API Documentation](https://docs.openharmony.cn/application-dev/developer-libs/napi)
- [ohos_nweb Documentation](../ohos_nweb/README.md)
