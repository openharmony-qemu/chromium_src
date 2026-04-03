# arkweb/patch

## Overview

The `patch` directory contains patch files for Chromium and its dependencies. These patches adapt Chromium to the OpenHarmony platform, fix bugs, and add specific features.

## Directory Structure

```
patch/
├── README.md                       # This document
├── CODEOWNERS                      # Code owners
├── build/                          # Build system patches
├── components/                     # Chromium component patches
└── third_party/                    # Third-party library patches
```

## Patch Categories

### 1. build/ - Build System Patches

Modifications and adaptations for the Chromium build system.

**Main Patch Content**:
- GN build script adaptation
- OpenHarmony toolchain integration
- Compiler option adjustments
- Linker configuration

**Example**:
```patch
# Adapt OpenHarmony platform detection
diff --git a/build/config/ozone.gni b/build/config/ozone.gni
index 1234567..abcdefg 100644
--- a/build/config/ozone.gni
+++ b/build/config/ozone.gni
@@ -10,6 +10,8 @@ declare_args() {
     ozone_platform = "wayland"
   } else if (is_win) {
     ozone_platform = "windows"
+  } else if (is_ohos) {
+    ozone_platform = "ohos"
   }
 }
```

### 2. components/ - Component Patches

Patches for various Chromium components.

**Main Components**:
- Network components
- Media components
- Rendering components
- Storage components
- UI components

**Typical Patch**:
```patch
# Fix network component compatibility on OpenHarmony
diff --git a/components/cronet/android/cronet_library.cc
```

### 3. third_party/ - Third-Party Library Patches

Patches for third-party dependencies.

**Common Libraries**:
- Skia (graphics library)
- ICU (internationalization)
- BoringSSL (crypto)
- FFmpeg (multimedia)
- V8 (JavaScript engine)
- Other dependencies

## Patch Management

### Patch Naming Convention

```
<sequence_number>-<short_description>.patch
```

For example:
```
0001-fix-ohos-build.patch
0002-add-ohos-platform-support.patch
0003-fix-network-stack.patch
```

### Patch Application Order

Patches are applied in sequence number order to ensure correct dependencies.

## Usage Guide

### Apply Patches

```bash
# Enter Chromium root directory
cd chromium_src

# Apply all patches
git apply ../arkweb/patch/*.patch

# Apply single patch
git apply ../arkweb/patch/0001-fix-ohos-build.patch
```

### Check Patch Status

```bash
# Check which patches are applied
git status | grep patch

# View patch details
git log --oneline | grep patch
```

### Rollback Patches

```bash
# Rollback specific patch
git apply -R ../arkweb/patch/0001-fix-ohos-build.patch
```

## Patch Types

### Platform Adaptation Patches

Patches that adapt Chromium to the OpenHarmony platform.

**Characteristics**:
- Add OpenHarmony platform detection
- Implement platform-specific interfaces
- Adjust compilation options

### Feature Enhancement Patches

Patches that add new features.

**Examples**:
- Add new APIs
- Extend existing features
- Integrate third-party services

### Bug Fix Patches

Patches that fix Chromium bugs.

**Types**:
- Memory leak fixes
- Crash fixes
- Compatibility issue fixes
- Performance issue fixes

### Optimization Patches

Performance and resource usage optimizations.

**Examples**:
- Reduce memory usage
- Improve rendering performance
- Optimize startup speed
- Lower power consumption

## Creating New Patches

### 1. Modify Code

Make modifications in the Chromium source code.

### 2. Generate Patch

```bash
# In Chromium root directory
git diff > ../arkweb/patch/0001-new-feature.patch
```

### 3. Add Description

Add description at the beginning of the patch file:

```patch
# Patch description
# Add XXX feature to support OpenHarmony platform
#
# Fixes: Bug ID
# Affects: Module name
# Tests: Test method

diff --git ...
```

### 4. Update README

Document the new patch in this README.

## Patch Conflicts

### Detect Conflicts

```bash
# Try to apply patch, conflicts will be shown
git apply --check ../arkweb/patch/0001-fix-ohos-build.patch
```

### Resolve Conflicts

1. Manually merge conflicting files
2. Update patch file
3. Test modified code
4. Update patch sequence number (if major change)

## Patch Maintenance

### Regular Updates

When Chromium upstream updates:
1. Check if patches are still needed
2. Update patches to adapt to new version
3. Test all patches
4. Update documentation

### Patch Lifecycle

1. **Create**: Add new patch
2. **Maintain**: Adapt to new versions
3. **Deprecate**: No longer needed (upstream fixed)

### Patch Strategy

- **Minimal**: Only modify necessary code
- **Isolated**: Keep patches independent from each other
- **Documented**: Record patch reasons
- **Upstream**: Try to submit fixes to upstream

## Known Patch List

### Core Platform Patches
- 0001: OpenHarmony platform base support
- 0002: Build system adaptation
- 0003: Toolchain integration

### Network Patches
- 0101: DNS resolution adaptation
- 0102: SSL/TLS integration
- 0103: Cookie storage adaptation

### Media Patches
- 0201: Audio output adaptation
- 0202: Video decoding integration
- 0203: Camera access support

### UI Patches
- 0301: Input method integration
- 0302: Touch event adaptation
- 0303: Window manager integration

### Third-Party Library Patches
- 1001: Skia adaptation
- 1002: ICU adaptation
- 1003: V8 adaptation

## Testing Patches

### Unit Tests

```bash
# Run relevant unit tests
ninja -C out/Default base_unittests
```

### Integration Tests

```bash
# Run integration tests
./build/run_integration_tests.sh
```

### Manual Testing

Manually verify functionality on OpenHarmony devices.

## Common Issues

### Q: Patch application fails?
A: Check if Chromium version matches, manually merge conflicts.

### Q: Patch causes build failure?
A: Check if patch is complete, review build errors.

### Q: How to determine which patches are still needed?
A: Regularly compare with upstream, remove merged patches.

## Maintainers

See CODEOWNERS file

## Related Documentation
- [Chromium Patch Guide](https://www.chromium.org/developers/contributing)
- [Git Patch Management](https://git-scm.com/docs/git-apply)
