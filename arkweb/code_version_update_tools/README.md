# arkweb/code_version_update_tools

## Overview

The `code_version_update_tools` directory contains tools for updating code versions in the ArkWeb project. This module manages Chromium version upgrades, dependency library version updates, and code synchronization.

## Directory Structure

```
code_version_update_tools/
├── README.md                       # This document
└── tools/                          # Version update scripts
    └── ...                         # Version update scripts
```

## Main Features

### 1. Chromium Version Upgrade

Automated Chromium version upgrade process.

**Upgrade Steps**:
1. Detect new version
2. Download source code
3. Apply patches
4. Update dependencies
5. Verify build
6. Run tests

**Usage**:
```bash
# Upgrade to specific version
python tools/upgrade_chromium.py --version 132.0.6834.123

# Upgrade to latest stable version
python tools/upgrade_chromium.py --latest
```

### 2. Dependency Version Management

Manage third-party dependency library versions.

**Functions**:
- Check dependency versions
- Update DEPS file
- Verify dependency compatibility
- Generate version reports

**Usage**:
```bash
# Check dependency versions
python tools/check_deps.py

# Update specific dependency
python tools/update_dep.py --name v8 --version 12.5.123

# Generate dependency report
python tools/dep_report.py --output deps_report.txt
```

### 3. Code Synchronization

Synchronize upstream Chromium code changes to ArkWeb.

**Sync Types**:
- Security patch synchronization
- Bug fix synchronization
- Feature update synchronization

**Usage**:
```bash
# Sync security patches
python tools/sync_security.py --cve CVE-2024-1234

# Sync specific fix
python tools/sync_fix.py --bug chromium:1234567
```

### 4. Patch Management

Manage application and update of Chromium patches.

**Functions**:
- Check patch status
- Update outdated patches
- Generate patch reports
- Conflict detection

**Usage**:
```bash
# Check patch status
python tools/check_patches.py

# Update patch
python tools/update_patch.py --patch 0001-fix-ohos-build.patch

# Detect patch conflicts
python tools/detect_conflicts.py
```

## Version Strategy

### Chromium Version Selection

**Selection Criteria**:
1. Stability priority
2. Security considerations
3. Performance requirements
4. Compatibility needs

**Version Types**:
- **LTS (Long Term Support)**: Long-term support version
- **Stable**: Stable version
- **Beta**: Test version (development only)

### Upgrade Cycle

**Recommended Cycle**:
- Minor version updates: Monthly
- Major version updates: Quarterly
- Emergency security updates: Immediate

## Tool Usage

### upgrade_chromium.py

Chromium version upgrade tool.

**Parameters**:
```bash
--version VERSION     # Specify version number
--latest              # Use latest stable version
--branch BRANCH       # Specify branch
--no-test             # Skip tests
--force               # Force upgrade
```

**Examples**:
```bash
# Upgrade to specific version
python tools/upgrade_chromium.py --version 132.0.6834.123

# Upgrade to latest and skip tests
python tools/upgrade_chromium.py --latest --no-test

# Force upgrade (ignore warnings)
python tools/upgrade_chromium.py --version 132.0.6834.123 --force
```

### check_deps.py

Dependency checking tool.

**Output**:
```
Dependency Check Report:
====================
v8: 12.5.123 ✅ (latest: 12.6.1)
skia: 123.m45 ✅ (latest: 123.m45)
icu: 74.1 ⚠️  (latest: 74.2)

Recommended updates: icu
```

### sync_security.py

Security patch synchronization tool.

**Parameters**:
```bash
--cve CVE-ID          # CVE code
--bug BUG-ID          # Chromium Bug ID
--commit COMMIT       # Commit hash
--auto                # Auto apply
```

**Examples**:
```bash
# Sync CVE patch
python tools/sync_security.py --cve CVE-2024-1234

# Sync specific commit
python tools/sync_security.py --commit abc1234def5678
```

## Configuration Files

### version_config.json

Version update configuration.

```json
{
  "chromium": {
    "current_version": "132.0.6834.123",
    "target_branch": "132",
    "auto_upgrade": false,
    "security_only": true
  },
  "dependencies": {
    "v8": {
      "version": "12.5.123",
      "auto_update": false
    },
    "skia": {
      "version": "123.m45",
      "auto_update": true
    }
  },
  "notifications": {
    "email": ["dev-team@example.com"],
    "webhook": "https://hooks.example.com/ci"
  }
}
```

## Workflow

### Version Upgrade Workflow

```
┌─────────────────────────────────────────────────────────────┐
│                    1. Preparation Phase                       │
│  - Check current version                                      │
│  - Analyze changes                                            │
│  - Assess impact scope                                        │
└────────────────────────┬────────────────────────────────────┘
                         │
                         ▼
┌─────────────────────────────────────────────────────────────┐
│                    2. Download Phase                          │
│  - Download Chromium source code                              │
│  - Update DEPS file                                           │
│  - Sync dependencies                                          │
└────────────────────────┬────────────────────────────────────┘
                         │
                         ▼
┌─────────────────────────────────────────────────────────────┐
│                    3. Patch Phase                             │
│  - Check existing patches                                     │
│  - Update conflicting patches                                 │
│  - Add new patches                                             │
└────────────────────────┬────────────────────────────────────┘
                         │
                         ▼
┌─────────────────────────────────────────────────────────────┐
│                    4. Build Phase                             │
│  - Clean old build                                            │
│  - Configure build options                                    │
│  - Compile all modules                                        │
└────────────────────────┬────────────────────────────────────┘
                         │
                         ▼
┌─────────────────────────────────────────────────────────────┐
│                    5. Test Phase                              │
│  - Run unit tests                                             │
│  - Run integration tests                                     │
│  - Performance benchmark tests                               │
└────────────────────────┬────────────────────────────────────┘
                         │
                         ▼
┌─────────────────────────────────────────────────────────────┐
│                    6. Verification Phase                      │
│  - Verify on test devices                                     │
│  - Compatibility tests                                       │
│  - Smoke tests                                                │
└────────────────────────┬────────────────────────────────────┘
                         │
                         ▼
┌─────────────────────────────────────────────────────────────┐
│                    7. Release Phase                           │
│  - Update version number                                     │
│  - Generate release notes                                     │
│  - Submit code                                                │
└─────────────────────────────────────────────────────────────┘
```

## Rollback Strategy

If upgrade fails, follow these steps to rollback:

1. **Immediate Rollback**:
   ```bash
   git revert <upgrade-commit>
   git revert <deps-commit>
   ```

2. **Verify Rollback**:
   ```bash
   # Rebuild
   ./arkweb/build/build.sh

   # Run tests
   ./arkweb/ut_tools/run_ut.py
   ```

3. **Notify Team**:
   - Send rollback notification
   - Document failure reasons
   - Plan next upgrade

## Monitoring and Reporting

### Version Monitoring

Regularly check version status:

```bash
# Check available updates
python tools/check_updates.py

# Generate version report
python tools/version_report.py
```

### Notification Mechanism

Automatically send notifications:

- New version available
- Security patch released
- Upgrade completion/failure

## Best Practices

### 1. Test Environment First

Upgrade in test environment first, verify before upgrading production environment.

### 2. Progressive Upgrades

- Upgrade minor versions first
- Verify stability before major version upgrades
- Keep rollback plan ready

### 3. Documentation Updates

Update documentation after each upgrade:
- Version change records
- API change descriptions
- Known issues list

### 4. Dependency Management

- Update dependency libraries promptly
- Check for security vulnerabilities
- Verify compatibility

## Common Issues

### Q: Build failure after upgrade?
A: Check if patches are correctly applied, update conflicting patches.

### Q: Tests fail?
A: Compare changes, determine if code or test modifications are needed.

### Q: Performance degradation?
A: Analyze performance data, may need to adjust configuration or optimize code.

## Maintainers

See project CODEOWNERS file

## Related Documentation
- [Chromium Release Calendar](https://www.chromium.org/developers/calendar)
- [DEPS File Format](https://www.chromium.org/developers/how-tos/get-the-code/working-with-checkout-deps)
