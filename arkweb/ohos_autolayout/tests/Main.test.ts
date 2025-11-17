/**
 * Unit tests for Main.ts - Complete coverage
 */

// Mock all dependencies before importing Main
const mockLog = {
  d: jest.fn(),
  e: jest.fn(),
  info: jest.fn(),
  w: jest.fn(),
};

const mockUtils = {
  isWideScreen: jest.fn(),
};

const mockCCMConfig = {
  fromJson: jest.fn(),
};

const mockFramework = {
  configReady: jest.fn(),
  stopFlag: false,
};

const mockWaitSystemReady = {
  configReady: jest.fn(),
};

jest.mock('../src/Debug/Log', () => ({
  __esModule: true,
  default: mockLog,
}));

jest.mock('../src/Framework/Common/Utils/Utils', () => ({
  __esModule: true,
  default: mockUtils,
}));

jest.mock('../src/Framework/Common/CCMConfig', () => ({
  CCMConfig: {
    getInstance: jest.fn(() => mockCCMConfig),
  },
}));

jest.mock('../src/Framework/Framework', () => ({
  __esModule: true,
  default: mockFramework,
}));

jest.mock('../src/Framework/Utils/WaitSystemReady', () => ({
  __esModule: true,
  default: mockWaitSystemReady,
}));

import { Main } from '../src/Main';

describe('Main Module - Complete Tests', () => {
  beforeEach(() => {
    // Reset all mocks before each test
    jest.clearAllMocks();
    Main.initFlag = false;
    mockFramework.stopFlag = false;
  });

  describe('Basic Structure', () => {
    test('Main class should be defined', () => {
      expect(Main).toBeDefined();
    });

    test('Main should have initFlag property', () => {
      expect(typeof Main.initFlag).toBe('boolean');
    });

    test('Main should have all required methods', () => {
      expect(typeof Main.start).toBe('function');
      expect(typeof Main.start_).toBe('function');
      expect(typeof Main.stop).toBe('function');
      expect(typeof Main.restart).toBe('function');
    });

    test('Main.initFlag should be initially false', () => {
      expect(Main.initFlag).toBe(false);
    });
  });

  describe('start() method', () => {
    test('should call start_() when CCMConfig.fromJson returns true', () => {
      mockCCMConfig.fromJson.mockReturnValue(true);
      const start_Spy = jest.spyOn(Main, 'start_');

      Main.start('valid config');

      expect(mockCCMConfig.fromJson).toHaveBeenCalledWith('valid config');
      expect(start_Spy).toHaveBeenCalled();
    });

    test('should not call start_() when CCMConfig.fromJson returns false', () => {
      mockCCMConfig.fromJson.mockReturnValue(false);
      const start_Spy = jest.spyOn(Main, 'start_');

      Main.start('invalid config');

      expect(mockCCMConfig.fromJson).toHaveBeenCalledWith('invalid config');
      expect(start_Spy).not.toHaveBeenCalled();
    });
  });

  describe('start_() method', () => {
    test('should initialize when wideScreen and not already initialized', () => {
      mockUtils.isWideScreen.mockReturnValue(true);
      Main.initFlag = false;

      Main.start_();

      expect(mockUtils.isWideScreen).toHaveBeenCalled();
      expect(mockWaitSystemReady.configReady).toHaveBeenCalledWith(mockFramework.configReady);
      expect(Main.initFlag).toBe(true);
    });

    test('should not initialize when wideScreen but already initialized', () => {
      mockUtils.isWideScreen.mockReturnValue(true);
      Main.initFlag = true;

      Main.start_();

      expect(mockUtils.isWideScreen).toHaveBeenCalled();
      expect(mockWaitSystemReady.configReady).not.toHaveBeenCalled();
      expect(Main.initFlag).toBe(true);
    });

    test('should not initialize when not wideScreen', () => {
      mockUtils.isWideScreen.mockReturnValue(false);
      Main.initFlag = false;

      Main.start_();

      expect(mockUtils.isWideScreen).toHaveBeenCalled();
      expect(mockWaitSystemReady.configReady).not.toHaveBeenCalled();
      expect(Main.initFlag).toBe(false);
    });
  });

  describe('stop() method', () => {
    test('should set Framework.stopFlag to true', () => {
      mockFramework.stopFlag = false;

      Main.stop();

      expect(mockFramework.stopFlag).toBe(true);
    });

    test('should set Framework.stopFlag to true even if already true', () => {
      mockFramework.stopFlag = true;

      Main.stop();

      expect(mockFramework.stopFlag).toBe(true);
    });
  });

  describe('restart() method', () => {
    test('should reset stopFlag, log message, and call start_', () => {
      mockFramework.stopFlag = true;
      const start_Spy = jest.spyOn(Main, 'start_');

      Main.restart();

      expect(mockFramework.stopFlag).toBe(false);
      expect(mockLog.d).toHaveBeenCalledWith('Autolayout reInit');
      expect(start_Spy).toHaveBeenCalled();
    });

    test('should work correctly when stopFlag is already false', () => {
      mockFramework.stopFlag = false;
      const start_Spy = jest.spyOn(Main, 'start_');

      Main.restart();

      expect(mockFramework.stopFlag).toBe(false);
      expect(mockLog.d).toHaveBeenCalledWith('Autolayout reInit');
      expect(start_Spy).toHaveBeenCalled();
    });
  });

  describe('Integration scenarios', () => {
    test('complete workflow: start with valid config on wideScreen', () => {
      mockCCMConfig.fromJson.mockReturnValue(true);
      mockUtils.isWideScreen.mockReturnValue(true);
      Main.initFlag = false;

      Main.start('valid config');

      expect(mockCCMConfig.fromJson).toHaveBeenCalledWith('valid config');
      expect(mockUtils.isWideScreen).toHaveBeenCalled();
      expect(mockWaitSystemReady.configReady).toHaveBeenCalledWith(mockFramework.configReady);
      expect(Main.initFlag).toBe(true);
    });

    test('stop and restart workflow', () => {
      mockUtils.isWideScreen.mockReturnValue(true);
      Main.initFlag = false;
      mockFramework.stopFlag = false;

      Main.stop();
      expect(mockFramework.stopFlag).toBe(true);

      Main.restart();
      expect(mockFramework.stopFlag).toBe(false);
      expect(mockLog.d).toHaveBeenCalledWith('Autolayout reInit');
      expect(mockUtils.isWideScreen).toHaveBeenCalled();
    });
  });
});
