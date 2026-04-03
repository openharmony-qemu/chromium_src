# arkweb/build

## Overview

The `build` directory contains the build system configuration, scripts, and tools for the ArkWeb project. This is the core directory for the entire ArkWeb compilation and build process.

## Directory Structure

```
build/
├── BUILD.gn                    # Main GN build configuration file
├── README.md                   # This document
├── build.sh                    # Main build script
├── build_web.sh                # Web component build script
├── prepare.sh                  # Preparation script
├── sign.sh                     # Signing script
├── upload.sh                   # Upload script
├── collect_prebuild_targets.py # Prebuild target collection script
├── copy_ndk_files.py           # NDK file copy script
├── include_vector_patch.py     # Vector patch inclusion script
├── ninja2trace.py              # Ninja to Trace conversion script
├── ut_upload.py                # Unit test upload script
├── assume_unchanged.json       # Git assume-unchanged configuration
├── CODEOWNERS                  # Code owners
├── build/                      # Build configuration subdirectory
├── config/                     # Configuration file directory
├── features/                   # Feature configuration
├── hap/                        # HAP package related
├── ohpm/                       # OHPM package management
├── search_engines/             # Search engine configuration
├── tarfile/                    # tar file processing
├── toolchain/                  # Toolchain configuration
└── unittests/                  # Unit test configuration
```

## Main Functional Modules

### 1. Build Scripts

- **build.sh**: Main build entry script, responsible for coordinating the entire compilation process
- **build_web.sh**: Dedicated build script for Web components
- **prepare.sh**: Pre-build preparation, including dependency checking, environment configuration, etc.
- **sign.sh**: Application package signing script

### 2. Python Tool Scripts

- **collect_prebuild_targets.py**: Collect and analyze prebuild targets
- **copy_ndk_files.py**: Copy and manage NDK files
- **include_vector_patch.py**: Process vector patches
- **ninja2trace.py**: Convert Ninja build logs to performance analysis traces
- **ut_upload.py**: Unit test result upload tool

### 3. Build Configuration

#### config/
Contains build system configuration files, defining compilation parameters, optimization options, etc.

#### features/
Feature toggle configuration, controlling which features are compiled into the final product.

#### toolchain/
Toolchain configuration, supporting multiple compilers and cross-compilation environments.

#### unittests/
Unit test related configuration, including three main test suites:
- `oh_base_unittests`: Base library unit tests
- `oh_core_unittests`: Core functionality unit tests
- `oh_smoke_unittests`: Smoke tests

### 4. Package Management

#### hap/
HarmonyOS Ability Package (HAP) configuration and build.

#### ohpm/
OpenHarmony Package Manager (OHPM) related configuration.

#### tarfile/
tar archive file processing related tools.

### 5. Other

#### search_engines/
Search engine configuration and integration.

## BUILD.gn Description

The main BUILD.gn file defines the following build targets:

```gn
# Test target group
group("arkweb_test_targets") {
    testonly = true
    deps = [
        "//arkweb/build/unittests:oh_base_unittests",
        "//arkweb/build/unittests:oh_core_unittests",
        "//arkweb/build/unittests:oh_smoke_unittests",
    ]
}

# Prebuild buildflags
group("arkweb_prebuild_buildflags") { ... }

# Prebuild mojom
group("arkweb_prebuild_mojom") { ... }
```

## Build Process

1. **Preparation Phase**: Execute `prepare.sh` to check environment and dependencies
2. **Configuration Phase**: GN generates ninja build files
3. **Compilation Phase**: ninja executes actual compilation
4. **Packaging Phase**: Generate HAP packages or other output formats
5. **Signing Phase**: Sign the output (if required)

## Main Configuration Options

- `arkweb_enable_prebuild`: Enable prebuild
- `buildflag_header_targets`: buildflag header targets
- `mojom_targets`: Mojom binding targets

## Maintainers

See CODEOWNERS file

## Related Documentation

- [GN Build System Documentation](https://gn.googlesource.com/gn/+/main/docs/reference.md)
- [Ninja Build System](https://ninja-build.org/)
