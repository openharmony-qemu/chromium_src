/**
 * Unit tests for Log.ts
 */
import Log, { LogLevel } from '../src/Debug/Log';

describe('Log Module', () => {
  let consoleLogSpy: jest.SpyInstance;
  let consoleInfoSpy: jest.SpyInstance;
  let consoleWarnSpy: jest.SpyInstance;
  let consoleErrorSpy: jest.SpyInstance;

  beforeEach(() => {
    // Reset console spies before each test
    consoleLogSpy = jest.spyOn(console, 'log').mockImplementation();
    consoleInfoSpy = jest.spyOn(console, 'info').mockImplementation();
    consoleWarnSpy = jest.spyOn(console, 'warn').mockImplementation();
    consoleErrorSpy = jest.spyOn(console, 'error').mockImplementation();
    
    // Reset log level
    Log.setLevel(LogLevel.DEBUG);
  });

  afterEach(() => {
    jest.clearAllMocks();
  });

  describe('LogLevel Management', () => {
    test('should set and get log level', () => {
      Log.setLevel(LogLevel.WARN);
      expect(Log.getLevel()).toBe(LogLevel.WARN);
      
      Log.setLevel(LogLevel.DEBUG);
      expect(Log.getLevel()).toBe(LogLevel.DEBUG);
      
      Log.setLevel(LogLevel.ERROR);
      expect(Log.getLevel()).toBe(LogLevel.ERROR);
    });
  });

  describe('DEBUG level logging', () => {
    test('should log debug message with default tag', () => {
      Log.d('Test debug message');
      
      expect(consoleLogSpy).toHaveBeenCalledTimes(1);
      const call = consoleLogSpy.mock.calls[0][0];
      expect(call).toContain('[DEBUG]');
      expect(call).toContain('Test debug message');
      expect(call).toMatch(/\[\d{2}:\d{2}:\d{2}\.\d{3}\]/);
    });

    test('should log debug message with custom tag', () => {
      Log.d('Test debug message', 'CustomTag');
      
      expect(consoleLogSpy).toHaveBeenCalledTimes(1);
      const call = consoleLogSpy.mock.calls[0][0];
      expect(call).toContain('[CustomTag]');
      expect(call).toContain('Test debug message');
    });

    test('should not log debug when level is INFO', () => {
      Log.setLevel(LogLevel.INFO);
      Log.d('Test debug message');
      
      expect(consoleLogSpy).not.toHaveBeenCalled();
    });

    test('should log debug with element info', () => {
      const mockElement = document.createElement('div');
      mockElement.className = 'test-class';
      
      Log.debug(mockElement, 'Element message');
      
      expect(consoleLogSpy).toHaveBeenCalledTimes(1);
      const firstCall = consoleLogSpy.mock.calls[0][0];
      expect(firstCall).toContain('DIV.test-class');
      expect(firstCall).toContain('Element message');
    });

    test('should log debug with null element', () => {
      Log.debug(null, 'Null element message');
      
      expect(consoleLogSpy).toHaveBeenCalledTimes(1);
      const firstCall = consoleLogSpy.mock.calls[0][0];
      expect(firstCall).toContain('null');
      expect(firstCall).toContain('Null element message');
    });

    test('should log debug with element without class', () => {
      const mockElement = document.createElement('span');
      
      Log.debug(mockElement, 'No class element');
      
      expect(consoleLogSpy).toHaveBeenCalledTimes(1);
      const firstCall = consoleLogSpy.mock.calls[0][0];
      expect(firstCall).toContain('SPAN.no-class');
    });
  });

  describe('INFO level logging', () => {
    test('should log info message', () => {
      Log.info('Test info message');
      
      expect(consoleInfoSpy).toHaveBeenCalledTimes(1);
      const call = consoleInfoSpy.mock.calls[0][0];
      expect(call).toContain('[INFO]');
      expect(call).toContain('Test info message');
    });

    test('should log info with custom tag', () => {
      Log.info('Info message', 'InfoTag');
      
      expect(consoleInfoSpy).toHaveBeenCalledTimes(1);
      const call = consoleInfoSpy.mock.calls[0][0];
      expect(call).toContain('[InfoTag]');
    });

    test('should not log info when level is WARN', () => {
      Log.setLevel(LogLevel.WARN);
      Log.info('Test info message');
      
      expect(consoleInfoSpy).not.toHaveBeenCalled();
    });

    test('should log info with element', () => {
      const mockElement = document.createElement('button');
      mockElement.className = 'btn-primary';
      
      Log.i(mockElement, 'Button clicked');
      
      expect(consoleInfoSpy).toHaveBeenCalledTimes(1);
      const call = consoleInfoSpy.mock.calls[0][0];
      expect(call).toContain('BUTTON.btn-primary');
      expect(call).toContain('Button clicked');
    });
  });

  describe('WARN level logging', () => {
    test('should log warning message', () => {
      Log.w('Test warning');
      
      expect(consoleWarnSpy).toHaveBeenCalledTimes(1);
      const call = consoleWarnSpy.mock.calls[0][0];
      expect(call).toContain('[WARN]');
      expect(call).toContain('Test warning');
    });

    test('should log warning with custom tag', () => {
      Log.w('Warning message', 'WarnTag');
      
      expect(consoleWarnSpy).toHaveBeenCalledTimes(1);
      const call = consoleWarnSpy.mock.calls[0][0];
      expect(call).toContain('[WarnTag]');
    });

    test('should not log warning when level is above ERROR', () => {
      // There's no level above ERROR in the enum, so WARN should still log at ERROR level
      Log.setLevel(LogLevel.ERROR);
      Log.w('Test warning');
      
      // WARN should not log when level is ERROR (ERROR is 3, WARN is 2)
      // Only ERROR level messages should log when level is set to ERROR
      expect(consoleWarnSpy).not.toHaveBeenCalled();
    });

    test('should log warning with element', () => {
      const mockElement = document.createElement('input');
      
      Log.warn(mockElement, 'Invalid input');
      
      expect(consoleWarnSpy).toHaveBeenCalledTimes(1);
      const call = consoleWarnSpy.mock.calls[0][0];
      expect(call).toContain('INPUT.no-class');
      expect(call).toContain('Invalid input');
    });
  });

  describe('ERROR level logging', () => {
    test('should log error message', () => {
      Log.e('Test error');
      
      expect(consoleErrorSpy).toHaveBeenCalledTimes(1);
      const call = consoleErrorSpy.mock.calls[0][0];
      expect(call).toContain('[ERROR]');
      expect(call).toContain('Test error');
    });

    test('should log error with custom tag', () => {
      Log.e('Error message', 'ErrorTag');
      
      expect(consoleErrorSpy).toHaveBeenCalledTimes(1);
      const call = consoleErrorSpy.mock.calls[0][0];
      expect(call).toContain('[ErrorTag]');
    });

    test('should log error with Error object', () => {
      const error = new Error('Something went wrong');
      Log.e('Error occurred', 'ErrorTag', error);
      
      expect(consoleErrorSpy).toHaveBeenCalledTimes(1);
      expect(consoleErrorSpy).toHaveBeenCalledWith(
        expect.stringContaining('Error occurred'),
        error
      );
    });

    test('should log error with element and error object', () => {
      const mockElement = document.createElement('form');
      const error = new Error('Form error');
      
      Log.error(mockElement, 'Form submission failed', 'FormTag', error);
      
      expect(consoleErrorSpy).toHaveBeenCalledTimes(1);
      const call = consoleErrorSpy.mock.calls[0][0];
      expect(call).toContain('FORM.no-class');
      expect(call).toContain('Form submission failed');
    });
  });

  describe('Timestamp formatting', () => {
    test('should include properly formatted timestamp', () => {
      Log.d('Test message');
      
      const call = consoleLogSpy.mock.calls[0][0];
      // Check format: [HH:MM:SS.mmm]
      expect(call).toMatch(/\[\d{2}:\d{2}:\d{2}\.\d{3}\]/);
    });
  });

  describe('Helper methods', () => {
    test('taskTimes should log performance message', () => {
      Log.taskTimes();
      
      expect(consoleLogSpy).toHaveBeenCalled();
    });

    test('showValue should log with element and key-value', () => {
      const mockElement = document.createElement('div');
      Log.showValue(mockElement, 'testKey', 'testValue');
      
      expect(consoleLogSpy).toHaveBeenCalled();
    });

    test('deleteValue should log deletion', () => {
      const mockElement = document.createElement('div');
      Log.deleteValue(mockElement, 'testKey');
      
      expect(consoleLogSpy).toHaveBeenCalled();
    });

    test('clearTag should log tag clearing', () => {
      const map = new Map<string, WeakMap<HTMLElement, string>>();
      Log.clearTag(map, 'testKey');
      
      expect(consoleLogSpy).toHaveBeenCalled();
    });

    test('logCaller should log caller info', () => {
      const mockElement = document.createElement('div');
      Log.logCaller(mockElement);
      
      expect(consoleLogSpy).toHaveBeenCalled();
    });

    test('checkState should log state check', () => {
      Log.checkState('testPosition');
      
      expect(consoleLogSpy).toHaveBeenCalled();
    });
  });

  describe('Log level filtering', () => {
    test('should respect log level hierarchy', () => {
      Log.setLevel(LogLevel.WARN);
      
      Log.d('Debug message');
      Log.info('Info message');
      Log.w('Warn message');
      Log.e('Error message');
      
      expect(consoleLogSpy).not.toHaveBeenCalled();
      expect(consoleInfoSpy).not.toHaveBeenCalled();
      expect(consoleWarnSpy).toHaveBeenCalled();
      expect(consoleErrorSpy).toHaveBeenCalled();
    });

    test('should filter debug when level is INFO', () => {
      Log.setLevel(LogLevel.INFO);
      
      Log.d('Debug message');
      Log.info('Info message');
      
      expect(consoleLogSpy).not.toHaveBeenCalled();
      expect(consoleInfoSpy).toHaveBeenCalled();
    });

    test('should filter debug and info when level is WARN', () => {
      Log.setLevel(LogLevel.WARN);
      
      Log.debug(null, 'Debug message');
      Log.i(null, 'Info message');
      Log.warn(null, 'Warn message');
      
      expect(consoleLogSpy).not.toHaveBeenCalled();
      expect(consoleInfoSpy).not.toHaveBeenCalled();
      expect(consoleWarnSpy).toHaveBeenCalled();
    });

    test('should only log errors when level is ERROR', () => {
      Log.setLevel(LogLevel.ERROR);
      
      Log.d('Debug message');
      Log.info('Info message');
      Log.w('Warn message');
      Log.e('Error message');
      
      expect(consoleLogSpy).not.toHaveBeenCalled();
      expect(consoleInfoSpy).not.toHaveBeenCalled();
      expect(consoleWarnSpy).not.toHaveBeenCalled();
      expect(consoleErrorSpy).toHaveBeenCalled();
    });
  });

  describe('Edge cases and additional coverage', () => {
    test('should log with undefined element', () => {
      Log.debug(undefined, 'Undefined element');
      
      expect(consoleLogSpy).toHaveBeenCalled();
      const call = consoleLogSpy.mock.calls[0][0];
      expect(call).toContain('null');
    });

    test('should handle error without Error object', () => {
      Log.e('Error message', 'ErrorTag');
      
      expect(consoleErrorSpy).toHaveBeenCalledWith(
        expect.stringContaining('Error message'),
        ''
      );
    });

    test('should handle error with element but without Error object', () => {
      const div = document.createElement('div');
      Log.error(div, 'Element error');
      
      expect(consoleErrorSpy).toHaveBeenCalledWith(
        expect.stringContaining('Element error'),
        ''
      );
    });

    test('helper methods should respect __DEV__ flag', () => {
      const div = document.createElement('div');
      const map = new Map<string, WeakMap<HTMLElement, string>>();
      
      Log.taskTimes();
      Log.showValue(div, 'key', 'value');
      Log.deleteValue(div, 'key');
      Log.clearTag(map, 'tag');
      Log.logCaller(div);
      Log.checkState('position');
      
      // All should call console.log in test environment
      expect(consoleLogSpy).toHaveBeenCalled();
    });

    test('should handle checkState without position', () => {
      Log.checkState();
      
      expect(consoleLogSpy).toHaveBeenCalled();
      const call = consoleLogSpy.mock.calls[0][0];
      expect(call).toContain('undefined');
    });
  });
});
