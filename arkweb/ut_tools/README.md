# arkweb/ut_tools

## Overview

`ut_tools` (Unit Test Tools) is the unit test toolkit for the ArkWeb project, providing test execution, result collection, data pushing, and email reporting capabilities. This module supports automated testing and result management for 48 test suites.

## Directory Structure

```
ut_tools/
├── __init__.py                     # Python package initialization
├── ut.py                           # Unit test main entry
├── run_ut.py                       # Test execution script
├── test_runner.py                  # Test runner
├── device_dependencies.py          # Device dependencies
├── host_path.py                    # Host path utilities
├── hdc_runner.py                   # HDC (Harmony Device Connector) runner
├── make_ut_filter.py               # Generate test filters
├── send_email.py                   # Send test report emails
├── update_code.sh                  # Update code script
├── ut_targets.json                 # Test target configuration
├── filters/                        # Test filter directory
├── push_data/                      # Test data push tools
│   ├── build_ut.sh                 # Build tests
│   ├── logger.py                   # Logging utilities
│   ├── push_testdata_config.json   # Push configuration
│   ├── push_testdata_filter.json   # Push filter
│   ├── push_testdata_tool.py       # Push tool
│   └── ut_data_deps/               # Test data dependencies
└── CODEOWNERS                      # Code owners
```

## Main Features

### 1. Test Execution (ut.py, run_ut.py)

Core test execution functionality, supporting both local and remote device testing.

**Main Functions**:
- Test suite discovery and execution
- Device connection management
- Test result collection
- Failure retry mechanism
- Parallel test support

### 2. Test Runner (test_runner.py)

Advanced test runner providing flexible test control.

**Features**:
- Custom test sequences
- Conditional test execution
- Test timeout control
- Resource cleanup

### 3. HDC Runner (hdc_runner.py)

Run tests on HarmonyOS devices via HDC tool.

**Functions**:
- Device discovery and connection
- File transfer
- Remote command execution
- Log collection

### 4. Device Dependencies (device_dependencies.py)

Manage device dependencies required for test execution.

### 5. Host Path (host_path.py)

Handle host system path operations.

### 6. Test Filter Generator (make_ut_filter.py)

Automatically generate test filters to exclude known failing test cases.

### 7. Email Reports (send_email.py)

Send test result email reports.

**Functions**:
- Test result summary
- Failure analysis
- Trend comparison
- HTML format emails

## Test Suite Configuration (ut_targets.json)

Configures 48 test suites, categorized as follows:

### Fully Passing Tests (28 suites)

| Test Suite | Test Target | Description |
|------------|-------------|-------------|
| accessibility_unittests | //ui/accessibility | Accessibility functionality tests |
| perfetto_unittests | //third_party/perfetto | Performance tracing tests |
| storage_unittests | //storage | Storage system tests |
| device_unittests | //device | Device access tests |
| liburlpattern_unittests | //third_party/liburlpattern | URL pattern tests |
| cast_unittests | //media/cast | Casting functionality tests |
| display_unittests | //ui/display | Display system tests |
| libjingle_xmpp_unittests | //third_party/libjingle_xmpp | XMPP tests |
| crypto_unittests | //crypto | Crypto functionality tests |
| midi_unittests | //media/midi | MIDI tests |
| color_unittests | //ui/color | Color processing tests |
| gwp_asan_unittests | //components/gwp_asan | GWP ASan tests |
| gin_unittests | //gin | Gin binding tests |
| filesystem_service_unittests | //components/services/filesystem | File system service tests |
| histogram_unittests | //third_party/catapult/tracing/tracing | Histogram tests |
| ced_unittests | //third_party/ced | CED tests |
| service_manager_unittests | //services/service_manager/tests | Service manager tests |
| latency_unittests | //ui/latency | Latency tests |
| native_theme_unittests | //ui/native_theme | Native theme tests |
| shell_dialogs_unittests | //ui/shell_dialogs | Shell dialog tests |
| blink_heap_unittests | //third_party/blink/renderer/platform/heap | Blink heap tests |
| media_mojo_unittests | //media/mojo | Media Mojo tests |
| libjpeg_turbo_unittests | //third_party/libjpeg_turbo | JPEG tests |
| skia_unittests | //skia | Skia graphics tests |
| gl_unittests | //ui/gl | OpenGL tests |
| ui_base_unittests | //ui/base | UI base tests |
| events_unittests | //ui/events | Event system tests |
| ui_touch_selection_unittests | //ui/touch_selection | Touch selection tests |
| printing_unittests | //printing | Printing functionality tests |
| gpu_unittests | //gpu | GPU tests |
| viz_unittests | //components/viz | Visualization compositing tests |
| gfx_unittests | //ui/gfx | Graphics base tests |
| sql_unittests | //sql | SQL tests (410/410 PASS) |
| url_unittests | //url | URL tests (146/146 PASS) |
| media_learning_mojo_unittests | //media/learning/mojo | Media learning tests (18/18 PASS) |
| zlib_unittests | //third_party/zlib | Zlib tests (77/77 PASS) |
| courgette_unittests | //courgette | Courgette tests (53/53 PASS) |
| blink_common_unittests | //third_party/blink/common | Blink common tests (2251/2251 PASS) |

### Tests Requiring Filters (14 suites)

These tests have some failures and require filters:

| Test Suite | Pass/Total | Description |
|------------|-----------|-------------|
| mojo_unittests | 1329/1329 | Mojo IPC tests |
| compositor_unittests | 191/204 | Compositor tests |
| base_unittests | 5948/5998 | Base library tests |
| media_unittests | 6081/6105 | Media framework tests |
| capture_unittests | 219/225 | Capture device tests |
| audio_unittests | 296/302 | Audio tests |

### Tests with Build Errors (4 suites)

These tests have build errors:

| Test Suite | Description |
|------------|-------------|
| components_unittests | Components tests |
| cc_unittests | Compositor tests |
| blink_unittests | Blink renderer tests |
| blink_platform_unittests | Blink platform tests |

## Test Filters

Filters located in `filters/` directory, used to exclude known failing test cases.

**Main Filters**:
- `base_unittests.filter`: Base library test filter
- `blink_unittests.filter`: Blink test filter
- `media_unittests.filter`: Media test filter
- `components_unittests.filter`: Components test filter
- And 30+ other filter files

## Test Data Pushing (push_data/)

### Build Tests (build_ut.sh)

Script to build test binaries.

### Push Tool (push_testdata_tool.py)

Push test data to remote servers or devices.

**Configuration Files**:
- `push_testdata_config.json`: Push configuration
- `push_testdata_filter.json`: Push filter

### Logging Tool (logger.py)

Unified logging utility.

## Usage Guide

### Run All Tests

```bash
python ut.py
```

### Run Specific Test Suite

```bash
python ut.py --suite base_unittests
```

### Generate Filters

```bash
python make_ut_filter.py
```

### Run Tests on Device

```bash
# Connect device
hdc list targets

# Push test files
python push_testdata_tool.py --push

# Run tests
python run_ut.py --device
```

### Send Test Report

```bash
python send_email.py --recipients user@example.com
```

## Configuration Options

### ut_targets.json Structure

```json
{
    "test_suite": "base_unittests",
    "test_target": "//base:base_unittests",
    "test_filter_file": "filters/base_unittests.filter"
}
```

### Environment Variables

- `UT_DEVICE`: Specify test device
- `UT_OUTPUT`: Test output directory
- `UT_FILTER`: Test filter file
- `UT_EMAIL`: Report receiving email

## Test Results

Test results include:
- Pass/fail statistics
- Failed test case details
- Execution time
- Memory/CPU usage
- Device information

## Continuous Integration

Can be integrated into CI/CD pipeline:

```yaml
# Example CI configuration
steps:
  - name: Run unit tests
    run: |
      python ut.py --ci
      python send_email.py --ci
```

## Common Issues

### Q: Some tests always fail?
A: Add the test case to the corresponding `.filter` file, excluding it using `--gtest_filter`.

### Q: How to add a new test suite?
A: Add configuration in `ut_targets.json` and create the corresponding filter file.

### Q: Device connection fails?
A: Check if HDC service is running and device is properly connected.

## Maintainers

See CODEOWNERS file

## Related Documentation
- [GTest Documentation](https://google.github.io/googletest/)
- [HDC Tool Documentation](https://docs.openharmony.cn/)
