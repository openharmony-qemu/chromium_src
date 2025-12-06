/**
 * Jest setup file
 * This file runs before all tests
 */

// Define global __DEV__ variable
// @ts-ignore
global.__DEV__ = true;

// Mock console methods to avoid cluttering test output
global.console = {
  ...console,
  log: jest.fn(),
  debug: jest.fn(),
  info: jest.fn(),
  warn: jest.fn(),
  error: jest.fn(),
};

// Setup DOM environment
Object.defineProperty(window, 'innerWidth', {
  writable: true,
  configurable: true,
  value: 1024,
});

Object.defineProperty(window, 'innerHeight', {
  writable: true,
  configurable: true,
  value: 768,
});

Object.defineProperty(window.screen, 'availWidth', {
  writable: true,
  configurable: true,
  value: 800,
});

Object.defineProperty(window.screen, 'availHeight', {
  writable: true,
  configurable: true,
  value: 600,
});
