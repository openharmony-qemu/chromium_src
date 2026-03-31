# arkweb/test

## Overview

The `test` directory contains the test framework and test cases for the ArkWeb project. This module provides unit tests, integration tests, performance tests, and other test types to ensure code quality and functional correctness.

## Directory Structure

```
test/
├── README.md                       # This document
├── CODEOWNERS                      # Code owners
├── BUILD.gn                        # GN build configuration
├── fuzztest/                       # Fuzz testing
│   └── ohos_nweb_fuzztest/         # NWEB fuzz tests
└── unittest/                       # Unit tests
    └── ohos_nweb_unittest/         # NWEB unit tests
```

## Test Types

### 1. Fuzz Testing (Fuzz Test)

Fuzz testing is an automated testing technique that discovers software defects by inputting random data.

#### ohos_nweb_fuzztest

Fuzz tests for the NWEB module.

**Test Targets**:
- API interface robustness
- Input validation
- Boundary conditions
- Security vulnerabilities

**Usage**:
```bash
# Run fuzz tests
./ohos_nweb_fuzztest --max_time=3600

# Use custom seeds
./ohos_nweb_fuzztest --seed_file=seeds.txt
```

### 2. Unit Testing (Unit Test)

Unit tests target individual functions, classes, or modules.

#### ohos_nweb_unittest

Unit tests for the NWEB module.

**Test Coverage**:
- CEF delegate layer
- NDK interface layer
- Download management
- Performance management
- Extension features

**Usage**:
```bash
# Run all unit tests
./ohos_nweb_unittest

# Run specific test
./ohos_nweb_unittest --gtest_filter=NWebTest.*

# Run with detailed logging
./ohos_nweb_unittest --gtest_output=xml:test_results.xml
```

## Build Configuration

### BUILD.gn

```gn
import("//build/test/test.gni")

# Fuzz test target
test("ohos_nweb_fuzztest") {
  sources = [
    "fuzztest/ohos_nweb_fuzztest.cc",
  ]

  deps = [
    "//arkweb/ohos_nweb",
    "//third_party/libfuzzer",
  ]

  configs += [ "//build/config/sanitizers:cfi_config" ]
}

# Unit test target
test("ohos_nweb_unittest") {
  sources = [
    "unittest/nweb_impl_unittest.cc",
    "unittest/nweb_delegate_unittest.cc",
    # ... more test files
  ]

  deps = [
    "//arkweb/ohos_nweb",
    "//testing/gtest",
  ]

  testonly = true
}
```

## Test Framework

### Google Test (GTest)

Mainly using Google Test as the unit testing framework.

**Basic Usage**:
```cpp
#include "gtest/gtest.h"

// Test case
TEST(NWebTest, LoadUrl) {
    auto nweb = NWeb::Create(delegate, context);
    ASSERT_NE(nweb, nullptr);

    nweb->LoadUrl("https://www.example.com");
    EXPECT_EQ(nweb->GetUrl(), "https://www.example.com");
}

// Test fixture
class NWebTest : public ::testing::Test {
protected:
    void SetUp() override {
        nweb_ = NWeb::Create(delegate_, context_);
    }

    void TearDown() override {
        nweb_.reset();
    }

    std::unique_ptr<NWeb> nweb_;
    NWebDelegate delegate_;
    NWebContext context_;
};

TEST_F(NWebTest, JavaScriptExecution) {
    nweb_->ExecuteJavaScript("1+1", callback);
    // Verify results...
}
```

### Google Mock (GMock)

Used to mock dependent objects.

```cpp
class MockNWebDelegate : public NWebDelegate {
public:
    MOCK_METHOD(void, OnLoadStarted, (const GURL& url), (override));
    MOCK_METHOD(void, OnLoadFinished, (const GURL& url), (override));
};

TEST(NWebTest, DelegateCallback) {
    MockNWebDelegate mock_delegate;
    EXPECT_CALL(mock_delegate, OnLoadStarted(_))
        .Times(1);

    auto nweb = NWeb::Create(mock_delegate, context);
    nweb->LoadUrl("https://www.example.com");
}
```

## Test Coverage

### NWEB Core Features

#### Engine Tests
- ✅ Engine initialization
- ✅ Engine shutdown
- ✅ Multi-instance management

#### Page Loading Tests
- ✅ URL loading
- ✅ Data loading
- ✅ History navigation
- ✅ Refresh and stop

#### JavaScript Tests
- ✅ Script execution
- ✅ Callback handling
- ✅ Object conversion
- ✅ Exception handling

#### Cookie Tests
- ✅ Cookie reading
- ✅ Cookie setting
- ✅ Cookie deletion
- ✅ Cookie management

#### Download Tests
- ✅ Download start
- ✅ Download progress
- ✅ Download completion
- ✅ Download cancellation

### CEF Delegate Layer Tests

#### Event Handling Tests
- ✅ Load events
- ✅ Render events
- ✅ Input events
- ✅ Focus events

#### Resource Handling Tests
- ✅ Resource loading
- ✅ Resource interception
- ✅ Custom protocols
- ✅ Cache control

#### Extension Feature Tests
- ✅ Bookmark management
- ✅ History
- ✅ Download management
- ✅ Tab management

### NDK Interface Tests

#### Native Object Tests
- ✅ Object creation
- ✅ Method calls
- ✅ Property access
- ✅ Object destruction

#### Callback Tests
- ✅ JavaScript callbacks
- ✅ Message callbacks
- ✅ Exception handling

## Test Data

### Test Resources

Data files required for testing:

```
test/
└── data/
    ├── html/              # HTML test files
    │   ├── test.html
    │   └── complex.html
    ├── javascript/        # JavaScript test files
    │   ├── test.js
    │   └── worker.js
    └── images/            # Image test files
        ├── test.png
        └── test.jpg
```

### Mock Data

```cpp
// Mock response data
const char* kMockHtmlResponse = R"(
<!DOCTYPE html>
<html>
<head><title>Test</title></head>
<body>Test Page</body>
</html>
)";

// Mock configuration
NWebConfiguration GetMockConfig() {
    NWebConfiguration config;
    config.enable_javascript = true;
    config.enable_cache = true;
    return config;
}
```

## Continuous Integration

### CI Configuration

Tests run automatically in CI pipeline:

```yaml
# .github/workflows/test.yml
steps:
  - name: Build tests
    run: |
      gn gen out/Release
      ninja -C out/Release arkweb_test_targets

  - name: Run tests
    run: |
      ./out/Release/ohos_nweb_unittest
      ./out/Release/ohos_nweb_fuzztest --max_time=600
```

### Test Reports

Test results automatically generate reports:

- JUnit XML format
- HTML coverage report
- Performance analysis report

## Performance Testing

### Benchmark Tests

```cpp
// Performance benchmark test
TEST(NWebPerfTest, PageLoadTime) {
    auto start = base::TimeTicks::Now();

    nweb->LoadUrl("https://www.example.com");
    WaitForLoadComplete();

    auto duration = base::TimeTicks::Now() - start;
    EXPECT_LT(duration.InSeconds(), 5);  // Complete within 5 seconds
}
```

### Memory Tests

```cpp
// Memory usage test
TEST(NWebMemTest, MemoryLeak) {
    size_t initial_memory = GetMemoryUsage();

    {
        auto nweb = NWeb::Create(delegate, context);
        nweb->LoadUrl("https://www.example.com");
        WaitForLoadComplete();
    }

    size_t final_memory = GetMemoryUsage();
    EXPECT_LT(final_memory - initial_memory, 1024 * 1024);  // 1MB
}
```

## Debugging Tests

### Enable Verbose Logging

```bash
# Run tests with verbose logging
./ohos_nweb_unittest --gtest_output=xml:results.xml --log-level=verbose
```

### Debug Failed Tests

```bash
# Only run failed tests
./ohos_nweb_unittest --gtest_filter=NWebTest.FailingTest:* --gmock_verbose=info
```

### Valgrind Detection

```bash
# Detect memory issues with Valgrind
valgrind --leak-check=full ./ohos_nweb_unittest
```

## Testing Best Practices

### 1. Test Naming

```cpp
// Good naming
TEST(NWebTest, LoadUrl_WithValidUrl_Succeeds)
TEST(NWebTest, LoadUrl_WithInvalidUrl_Fails)

// Avoid naming
TEST(NWebTest, Test1)
TEST(NWebTest, LoadTest)
```

### 2. Test Structure

Use AAA pattern (Arrange-Act-Assert):

```cpp
TEST(NWebTest, LoadUrl) {
    // Arrange: Prepare test
    auto nweb = NWeb::Create(delegate, context);
    GURL url("https://www.example.com");

    // Act: Execute operation
    nweb->LoadUrl(url);

    // Assert: Verify result
    EXPECT_EQ(nweb->GetUrl(), url);
}
```

### 3. Test Isolation

Each test should be independent:

```cpp
TEST(NWebTest, Test1) {
    // Does not depend on Test2 state
    auto nweb = NWeb::Create(delegate, context);
    // ...
}
```

## Common Issues

### Q: Test timeout?
A: Use `--gtest_timeout` to increase timeout, or optimize test code.

### Q: How to create mock objects?
A: Use Google Mock to define interfaces and create mock classes.

### Q: How to test async code?
A: Use message loop run or callback waiting mechanisms.

## Maintainers

See CODEOWNERS file

## Related Documentation
- [Google Test Documentation](https://google.github.io/googletest/)
- [Google Mock Documentation](https://google.github.io/googlemock/)
- [Fuzz Testing Guide](https://github.com/google/fuzzing)
