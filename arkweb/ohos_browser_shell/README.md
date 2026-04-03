# arkweb/ohos_browser_shell

## Overview

`ohos_browser_shell` is a browser application shell based on OpenHarmony, a complete HarmonyOS application that showcases and tests ArkWeb's web rendering capabilities. This app provides basic browser UI and interaction functionality.

## Directory Structure

```
ohos_browser_shell/
├── .gitignore.backup              # Git ignore configuration backup
├── BUILD.gn                        # GN build configuration
├── build-profile.json5             # HarmonyOS build configuration
├── CODEOWNERS                      # Code owners
├── oh-package.json5                # OHPM package configuration
├── oh-package-lock.json5           # OHPM lock file
├── package.json                    # NPM package configuration
├── hvigorfile.js                   # Hvigor build script
├── hvigorw                         # Hvigor wrapper script (Unix)
├── hvigorw.bat                     # Hvigor wrapper script (Windows)
├── AppScope/                       # Application scope resources
│   └── app.json5                   # Application configuration
├── entry/                          # Application entry module
│   └── src/                        # Source code
└── hvigor/                         # Hvigor plugins and configuration
```

## Application Architecture

### HarmonyOS Application Structure

This app follows the standard HarmonyOS application structure:

```
ohos_browser_shell/
├── AppScope/           # Application global configuration and resources
│   ├── app.json5       # Application-level configuration file
│   └── resources/      # Application-level resources
└── entry/              # Main module
    ├── src/            # Source code
    │   ├── main/       # Main entry
    │   │   ├── ets/    # ArkTS code
    │   │   └── resources/ # Resource files
    │   └── mock/       # Mock data
    └── build/          # Build output
```

## Core Components

### 1. Application Configuration (AppScope/app.json5)

Application-level configuration file, defining basic application information.

**Main Configuration**:
```json5
{
  "app": {
    "bundleName": "com.ohos.arkweb.browser",
    "vendor": "Huawei",
    "versionCode": 1000000,
    "versionName": "1.0.0",
    "icon": "$media:app_icon",
    "label": "$string:app_name"
  }
}
```

### 2. Module Configuration (entry/src/main/module.json5)

Main module configuration file.

### 3. Hvigor Build System

**hvigorfile.js**: Hvigor build script, defining build tasks.

**hvigorw/hvigorw.bat**: Hvigor command-line tool wrapper scripts.

## Build Configuration

### build-profile.json5

HarmonyOS application build configuration file.

**Main Configuration**:
```json5
{
  "apiType": "stageMode",
  "buildOption": {},
  "buildModeSet": [
    {
      "name": "debug",
      "runtimeOS": "HarmonyOS"
    },
    {
      "name": "release",
      "runtimeOS": "HarmonyOS"
    }
  ],
  "targets": [
    {
      "name": "default"
    }
  ]
}
```

### BUILD.gn

Chromium GN build system integration.

```gn
# Define browser application build targets
```

## Package Management

### OHPM (OpenHarmony Package Manager)

**oh-package.json5**: OpenHarmony Package Manager configuration.

**oh-package-lock.json5**: Dependency version lock file.

### NPM

**package.json**: Node.js package manager configuration for frontend resource builds.

## UI Components

### Main Page (MainPage)

Application main interface, including:
- Web view container
- Navigation bar
- Address bar
- Toolbar

### Web View Integration

Integrates ArkWeb's web rendering capabilities:

```typescript
import webview from '@ohos.web.webview';

// Create Web component
@Entry
@Component
struct BrowserPage {
  build() {
    Column() {
      Web({ src: "https://www.example.com" })
        .onErrorReceive((event) => {
          // Error handling
        })
    }
  }
}
```

## Features

### Basic Browser Functionality

- ✅ Page loading and rendering
- ✅ Forward/back navigation
- ✅ Page refresh
- ✅ URL input and navigation
- ✅ Basic bookmark functionality

### Advanced Features

- 🔧 Multi-tab support
- 🔧 History
- 🔧 Download management
- 🔧 Cookie management
- 🔧 Developer tools integration
- 🔧 Custom user agent

## Build and Run

### Build Application

```bash
# Build using Hvigor
./hvigorw assembleHap

# Or using npm
npm run build
```

### Run on Device

```bash
# Install HAP package
hdc install entry/build/default/outputs/default/entry-default-signed.hap

# Launch application
hdc shell aa start -a BrowserAbility -b com.ohos.arkweb.browser
```

### Debugging

```bash
# View logs
hdc shell hilog -T BrowserShell

# Remote debugging
# Enable DevTools port
```

## Configuration Options

### Application Configuration

Configure in `AppScope/app.json5`:
- Application name and icon
- Version information
- Permission declarations

### Module Configuration

Configure in `entry/src/main/module.json5`:
- Ability configuration
- Permission requests
- Metadata

## Permissions Required

Application requires the following permissions:

```json
{
  "requestPermissions": [
    {
      "name": "ohos.permission.INTERNET"
    },
    {
      "name": "ohos.permission.GET_NETWORK_INFO"
    }
  ]
}
```

## Testing

### Unit Tests

```bash
# Run unit tests
npm test
```

### Integration Tests

```bash
# Run tests on device
hdc test ...
```

## Development Guide

### Adding New Features

1. Create new component in `entry/src/main/ets`
2. Register in router
3. Update UI layout
4. Add necessary permissions

### Modifying Styles

Edit ArkTS style files:
```typescript
@Styles
struct CustomStyle {
  .backgroundColor(Color.White)
  .borderRadius(8)
}
```

### Integrating ArkWeb

Use NWeb SDK:
```typescript
import nweb from '@ohos.nweb';

// Create NWeb instance
const web = nweb.create();
```

## Known Issues

1. Some advanced web APIs not fully supported
2. Some website compatibility issues
3. High memory usage

## Future Plans

- [ ] Improve tab management
- [ ] Add private browsing mode
- [ ] Support plugin system
- [ ] Optimize performance and memory usage
- [ ] Enhance developer tools

## Maintainers

See CODEOWNERS file

## Related Documentation
- [HarmonyOS Application Development Documentation](https://docs.openharmony.cn/)
- [ArkTS Language Guide](https://docs.openharmony.cn/)
- [Hvigor Build Tool](https://docs.openharmony.cn/)
