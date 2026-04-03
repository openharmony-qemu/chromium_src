# arkweb/ci_tools

## Overview

The `ci_tools` directory contains the continuous integration (CI) toolkit for the ArkWeb project. This module provides code checking, build verification, automated testing, and other functionalities to ensure code quality and automated release processes.

## Directory Structure

```
ci_tools/
├── README.md                       # This document
├── CODEOWNERS                      # Code owners
└── tools/                          # CI tool scripts
    └── ...                         # Various tools
```

## Main Features

### 1. Code Checking (check.py)

Static code analysis tool that checks code quality and standards.

**Check Items**:
- Code style
- Naming conventions
- File header comments
- Copyright notices
- API usage standards

**Usage**:
```bash
# Check all modified files
python ci_tools/check.py

# Check specific directory
python ci_tools/check.py --dir arkweb/ohos_nweb

# Only check specific file types
python ci_tools/check.py --ext cc,h
```

### 2. Build Verification

Automated build verification tools.

**Functions**:
- Configuration validation
- Dependency checking
- Compilation testing
- Link verification

### 3. Test Integration

Integrate automated testing into CI pipeline.

**Test Types**:
- Unit tests
- Integration tests
- Performance tests
- Security tests

### 4. Release Process

Automated release tools.

**Process**:
1. Version number generation
2. Changelog generation
3. Package building
4. Signature verification
5. Release deployment

## CI Workflow

### PR Check Process

```
┌─────────────────────────────────────────────────────────────┐
│                    Submit Pull Request                        │
└────────────────────────┬────────────────────────────────────┘
                         │
                         ▼
┌─────────────────────────────────────────────────────────────┐
│                    Code Check (check.py)                      │
│  - Code style                                                │
│  - Copyright notice                                          │
│  - Naming conventions                                        │
└────────────────────────┬────────────────────────────────────┘
                         │ Pass
                         ▼
┌─────────────────────────────────────────────────────────────┐
│                    Build Verification                         │
│  - Configuration check                                       │
│  - Dependency verification                                    │
└────────────────────────┬────────────────────────────────────┘
                         │ Pass
                         ▼
┌─────────────────────────────────────────────────────────────┐
│                    Compilation Test                           │
│  - Compile all modules                                       │
│  - Check compiler warnings                                   │
└────────────────────────┬────────────────────────────────────┘
                         │ Pass
                         ▼
┌─────────────────────────────────────────────────────────────┐
│                    Run Tests                                  │
│  - Unit tests                                                │
│  - Integration tests                                        │
└────────────────────────┬────────────────────────────────────┘
                         │ Pass
                         ▼
┌─────────────────────────────────────────────────────────────┐
│                    Code Review                                │
│  - Manual review                                             │
│  - Security review                                           │
└────────────────────────┬────────────────────────────────────┘
                         │ Pass
                         ▼
┌─────────────────────────────────────────────────────────────┐
│                    Merge Code                                 │
└─────────────────────────────────────────────────────────────┘
```

### Release Process

```
┌─────────────────────────────────────────────────────────────┐
│                   Create Release Branch                       │
└────────────────────────┬────────────────────────────────────┘
                         │
                         ▼
┌─────────────────────────────────────────────────────────────┐
│                   Version Preparation                         │
│  - Update version numbers                                    │
│  - Generate changelog                                        │
│  - Update documentation                                      │
└────────────────────────┬────────────────────────────────────┘
                         │
                         ▼
┌─────────────────────────────────────────────────────────────┐
│                   Full Build                                  │
│  - Clean build                                               │
│  - Compile all targets                                      │
│  - Run all tests                                             │
└────────────────────────┬────────────────────────────────────┘
                         │ Pass
                         ▼
┌─────────────────────────────────────────────────────────────┐
│                   Package Build                                │
│  - Create HAP packages                                      │
│  - Create installation packages                             │
│  - Signature verification                                    │
└────────────────────────┬────────────────────────────────────┘
                         │
                         ▼
┌─────────────────────────────────────────────────────────────┐
│                   Release Verification                        │
│  - Verify on test devices                                   │
│  - Smoke tests                                               │
└────────────────────────┬────────────────────────────────────┘
                         │ Pass
                         ▼
┌─────────────────────────────────────────────────────────────┐
│                   Release Deployment                          │
│  - Upload to release repo                                    │
│  - Send release notifications                                │
│  - Update documentation                                      │
└─────────────────────────────────────────────────────────────┘
```

## Configuration Files

### CI Configuration

CI system configuration files (location may vary by platform):

**GitHub Actions** (`.github/workflows/ci.yml`):
```yaml
name: ArkWeb CI

on:
  push:
    branches: [ main, develop ]
  pull_request:
    branches: [ main ]

jobs:
  check:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v2
      - name: Run code check
        run: python ci_tools/check.py

  build:
    needs: check
    runs-on: ubuntu-latest
    steps:
      - name: Build
        run: |
          ./arkweb/build/prepare.sh
          gn gen out/Release
          ninja -C out/Release
```

**Jenkins** (`Jenkinsfile`):
```groovy
pipeline {
    agent any

    stages {
        stage('Check') {
            steps {
                sh 'python ci_tools/check.py'
            }
        }

        stage('Build') {
            steps {
                sh './arkweb/build/build.sh'
            }
        }

        stage('Test') {
            steps {
                sh './arkweb/ut_tools/run_ut.py'
            }
        }
    }
}
```

## Tool Scripts

### check.py

Code checking tool.

**Functions**:
- Check file header comments
- Check copyright notices
- Check code style
- Check naming conventions

**Configuration**:
```python
# Configuration options
config = {
    'copyright_years': '2022-2025',
    'allowed_owners': ['@huawei.com'],
    'file_extensions': ['.cc', '.h', '.ts', '.js'],
    'exclude_dirs': ['third_party', 'out'],
}
```

### Other Tools

#### Build Tools
- `build.sh`: Build script
- `prepare.sh`: Environment preparation
- `sign.sh`: Signing tool

#### Test Tools
- `run_tests.sh`: Test execution script
- `collect_coverage.sh`: Coverage collection

#### Release Tools
- `create_release.sh`: Create release
- `upload_artifacts.sh`: Upload build artifacts

## Quality Gates

### Must-Pass Checks

1. **Code Style**: Pass all code style checks
2. **Build Success**: No compilation errors
3. **Unit Tests**: All unit tests pass
4. **Integration Tests**: Core functionality tests pass
5. **Security Scan**: No high-severity security vulnerabilities

### Optional Checks

1. **Performance Benchmark**: Performance does not degrade by more than 5%
2. **Memory Leaks**: No memory leaks
3. **Code Coverage**: New code coverage > 80%

## Status Reports

### Build Status

Generate status report for each build:

```
Build ID: #1234
Status: ✅ Success
Branch: main
Commit: abc1234 (Fix navigation bug)

Check Results:
✅ Code style
✅ Build
✅ Unit tests (48/48)
✅ Integration tests

Performance:
- Build time: 15min
- Test time: 8min
- Total time: 25min
```

### Test Reports

```
Test Suite: ohos_nweb_unittests
Result: ✅ Pass
Pass/Fail: 152/0
Coverage: 85.3%

Failed Tests: None
```

## Troubleshooting

### Common Issues

#### Q: Code check fails?
A: Review check report, fix non-compliant items.

#### Q: Build fails?
A: Review build logs, check compilation errors and warnings.

#### Q: Tests fail?
A: Review test logs, locate failing test cases.

### Log Locations

- Check logs: `build/logs/check.log`
- Build logs: `build/logs/build.log`
- Test logs: `build/logs/test.log`

## Best Practices

### 1. Local Checks Before Commit

```bash
# Run checks locally
python ci_tools/check.py

# Build locally
./arkweb/build/build.sh

# Run tests locally
./arkweb/ut_tools/run_ut.py
```

### 2. Follow Code Standards

- Add proper file header comments
- Follow naming conventions
- Write unit tests
- Update documentation

### 3. Small Commits

- Commit small changes frequently
- Each commit completes one feature
- Easier to locate issues

### 4. Fix CI Failures Promptly

- Prioritize fixing CI failures
- Don't skip CI checks
- Notify relevant personnel

## Maintainers

See CODEOWNERS file

## Related Documentation
- [CI/CD Best Practices](https://www.atlassian.com/continuous-delivery/principles/continuous-integration-vs-delivery-vs-deployment)
- [GitHub Actions Documentation](https://docs.github.com/en/actions)
