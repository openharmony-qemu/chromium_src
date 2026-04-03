# arkweb/ohos_autolayout

## Overview

`ohos_autolayout` is an intelligent auto-layout module in ArkWeb, implemented in TypeScript/JavaScript. This module provides intelligent page layout recognition, detection, and reconstruction capabilities, automatically identifying UI elements like popups, lists, and cards in web pages and performing adaptive layout adjustments.

## Directory Structure

```
ohos_autolayout/
├── BUILD.gn                        # GN build configuration
├── README.md                       # This document
├── package.json                    # NPM package configuration
├── tsconfig.json                   # TypeScript configuration
├── jest.config.js                  # Jest test configuration
├── autolayout_resources.grd        # Resource file configuration
├── build/                          # Build configuration
│   └── umd/                        # UMD build configuration
├── dist/                           # Build output directory
├── src/                            # Source code
│   ├── Main.ts                     # Main entry point
│   ├── Debug/                      # Debug tools
│   └── Framework/                  # Framework core
│       └── Popup/                  # Popup detection framework
├── tests/                          # Test files
└── build/                          # Webpack build
```

## Core Features

### 1. Popup Detection and Layout (Framework/Popup/)

Intelligent recognition and handling of various popup and overlay types.

**Core Components**:

#### PopupWindowDetector
Popup window detector that identifies popup elements on pages.

**Features**:
- Detect modal popups
- Detect non-modal overlays
- Detect sidebars
- Detect dropdown menus

#### PopupRecog
Popup recognition engine.

**Functions**:
- Feature extraction
- Pattern matching
- Type classification

#### PopupDecisionTree
Popup decision tree for determining popup types and layout strategies.

#### PopupStateManager
Popup state manager tracking popup open/close states.

#### PopupWindowRelayout
Popup window reconstructor that automatically adjusts popup layouts.

#### PopupLayoutState
Popup layout state management.

#### PopupType
Popup type definitions.

#### PopupInfo
Popup information data structure.

#### PredictionResult
Prediction result data structure.

### 2. Alphabet Index Navigation (Alphabet/)

Alphabet index navigation component.

**Main Files**:
- `Main.js`: Main entry point
- `AlphabetAdapter.js`: Alphabet adapter
- `AlphabetButtonGroup.js`: Button group
- `AlphabetButtonBox.js`: Button container
- `AlphabetMicroDots.js`: Micro-dot indicators
- `Conf.js`: Configuration file
- `Utils.js`: Utility functions

### 3. Debug Tools (Debug/)

Development and debugging assistance tools.

**Components**:
- `Log.ts`: Logging system
- `Tag.ts`: Tag system

## Tech Stack

### Development Languages
- **TypeScript**: Main development language
- **JavaScript**: Some modules

### Build Tools
- **Webpack**: Module bundling
- **Babel**: ES6+ transpilation
- **Terser**: Code minification

### Test Framework
- **Jest**: Unit testing
- **jsdom**: DOM simulation

### Development Dependencies

```json
{
  "@babel/plugin-transform-modules-umd": "^7.25.9",
  "@types/jest": "^30.0.0",
  "@types/jsdom": "^27.0.0",
  "cross-env": "^7.0.3",
  "dts-bundle-generator": "^6.13.0",
  "jest": "^30.2.0",
  "jest-environment-jsdom": "^30.2.0",
  "jsdom": "^27.0.0",
  "prettier": "^2.8.8",
  "terser-webpack-plugin": "^5.3.14",
  "ts-jest": "^29.4.5",
  "typescript": "^4.9.5",
  "webpack": "^5.98.0",
  "webpack-cli": "^4.10.0",
  "webpack-merge": "^6.0.1",
  "webpack-shell-plugin-next": "^2.3.2",
  "ts-loader": "^9.5.2"
}
```

### Runtime Dependencies

```json
{
  "express": "^4.21.2",
  "formidable": "^2.1.2"
}
```

## NPM Scripts

### Development
```bash
npm run dev
# Build UMD module using webpack config (development mode)
```

### Production
```bash
npm run release
# Build UMD module with NODE_ENV=production (production mode)
```

### Testing
```bash
npm test                # Run tests
npm run test:watch      # Run tests in watch mode
npm run test:coverage   # Generate test coverage report
```

## Build Output

Build artifacts located in `dist/` directory, including:
- UMD format JavaScript files
- TypeScript type definition files (.d.ts)
- Source maps (development mode)

## Integration

### Using in NWEB

```cpp
// Enable auto layout in nweb
if (arkweb_autolayout) {
    // Load auto layout resources
    // Inject auto layout scripts
}
```

### Resource Bundling

Configure resource bundling via `autolayout_resources.grd`:

```grd
<!-- Define auto layout related resource files -->
```

Generated resource targets:
```gn
//arkweb/ohos_autolayout:autolayout_resources
```

## Development Guide

### Adding New Popup Types

1. Define new type in `PopupType.ts`
2. Add recognition logic in `PopupRecog.ts`
3. Add decision rules in `PopupDecisionTree.ts`
4. Implement layout adjustment in `PopupWindowRelayout.ts`
5. Write unit tests

### Debugging

Enable debug logging:
```typescript
import { Log } from './Debug/Log';

Log.enableDebug();
Log.info('Debug message');
```

### Testing

Write Jest tests:
```typescript
describe('PopupDetector', () => {
    it('should detect modal popup', () => {
        // Test code
    });
});
```

## Configuration Options

### Build Configuration

**tsconfig.json**:
```json
{
  "compilerOptions": {
    "target": "ES6",
    "module": "UMD",
    "declaration": true,
    "outDir": "./dist"
  }
}
```

**jest.config.js**:
```javascript
module.exports = {
    preset: 'ts-jest',
    testEnvironment: 'jsdom',
    // ...
};
```

## Performance Optimization

1. **Code Splitting**: Use Webpack code splitting to reduce initial load
2. **Tree Shaking**: Remove unused code
3. **Minification**: Use Terser to minify production code
4. **Caching**: Leverage Webpack persistent caching

## Known Issues

1. Some complex popups may not be correctly recognized
2. Detection of dynamically loaded content has delays
3. Some custom framework components have low recognition rates

## Future Plans

- [ ] Enhance popup detection accuracy
- [ ] Support more UI pattern recognition
- [ ] Add machine learning models for assisted recognition
- [ ] Optimize performance, reduce memory usage
- [ ] Support custom rule configuration

## Maintainers

See project CODEOWNERS file

## Related Documentation
- [TypeScript Documentation](https://www.typescriptlang.org/)
- [Jest Documentation](https://jestjs.io/)
- [Webpack Documentation](https://webpack.js.org/)
