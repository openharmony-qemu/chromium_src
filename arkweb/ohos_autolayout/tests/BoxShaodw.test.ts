/**
 * Unit tests for BoxShaodw.ts
 * 测试 BoxShadow 接口类型定义
 */

import { BoxShadow } from '../src/Framework/Common/BoxShadow';

describe('BoxShadow Interface', () => {
  describe('Type Definition', () => {
    test('should accept valid BoxShadow object', () => {
      const validBoxShadow: BoxShadow = {
        offsetX: '10px',
        offsetY: '20px',
        blurRadius: '5px',
        spreadRadius: '0px',
        color: 'rgba(0, 0, 0, 0.5)',
      };

      expect(validBoxShadow.offsetX).toBe('10px');
      expect(validBoxShadow.offsetY).toBe('20px');
      expect(validBoxShadow.blurRadius).toBe('5px');
      expect(validBoxShadow.spreadRadius).toBe('0px');
      expect(validBoxShadow.color).toBe('rgba(0, 0, 0, 0.5)');
    });

    test('should accept BoxShadow with zero values', () => {
      const zeroBoxShadow: BoxShadow = {
        offsetX: '0px',
        offsetY: '0px',
        blurRadius: '0px',
        spreadRadius: '0px',
        color: 'transparent',
      };

      expect(zeroBoxShadow.offsetX).toBe('0px');
      expect(zeroBoxShadow.offsetY).toBe('0px');
      expect(zeroBoxShadow.blurRadius).toBe('0px');
      expect(zeroBoxShadow.spreadRadius).toBe('0px');
      expect(zeroBoxShadow.color).toBe('transparent');
    });

    test('should accept BoxShadow with negative values', () => {
      const negativeBoxShadow: BoxShadow = {
        offsetX: '-5px',
        offsetY: '-10px',
        blurRadius: '3px',
        spreadRadius: '-2px',
        color: '#000000',
      };

      expect(negativeBoxShadow.offsetX).toBe('-5px');
      expect(negativeBoxShadow.offsetY).toBe('-10px');
      expect(negativeBoxShadow.blurRadius).toBe('3px');
      expect(negativeBoxShadow.spreadRadius).toBe('-2px');
      expect(negativeBoxShadow.color).toBe('#000000');
    });

    test('should accept BoxShadow with different color formats', () => {
      const hexColor: BoxShadow = {
        offsetX: '0px',
        offsetY: '0px',
        blurRadius: '0px',
        spreadRadius: '0px',
        color: '#FF5733',
      };

      const rgbColor: BoxShadow = {
        offsetX: '0px',
        offsetY: '0px',
        blurRadius: '0px',
        spreadRadius: '0px',
        color: 'rgb(255, 87, 51)',
      };

      const rgbaColor: BoxShadow = {
        offsetX: '0px',
        offsetY: '0px',
        blurRadius: '0px',
        spreadRadius: '0px',
        color: 'rgba(255, 87, 51, 0.8)',
      };

      const namedColor: BoxShadow = {
        offsetX: '0px',
        offsetY: '0px',
        blurRadius: '0px',
        spreadRadius: '0px',
        color: 'black',
      };

      expect(hexColor.color).toBe('#FF5733');
      expect(rgbColor.color).toBe('rgb(255, 87, 51)');
      expect(rgbaColor.color).toBe('rgba(255, 87, 51, 0.8)');
      expect(namedColor.color).toBe('black');
    });

    test('should accept BoxShadow with different unit types', () => {
      const remUnits: BoxShadow = {
        offsetX: '1rem',
        offsetY: '2rem',
        blurRadius: '0.5rem',
        spreadRadius: '0rem',
        color: 'black',
      };

      const emUnits: BoxShadow = {
        offsetX: '1em',
        offsetY: '2em',
        blurRadius: '0.5em',
        spreadRadius: '0em',
        color: 'black',
      };

      expect(remUnits.offsetX).toBe('1rem');
      expect(emUnits.offsetX).toBe('1em');
    });

    test('should have all required properties', () => {
      const boxShadow: BoxShadow = {
        offsetX: '0px',
        offsetY: '0px',
        blurRadius: '0px',
        spreadRadius: '0px',
        color: 'black',
      };

      expect(boxShadow).toHaveProperty('offsetX');
      expect(boxShadow).toHaveProperty('offsetY');
      expect(boxShadow).toHaveProperty('blurRadius');
      expect(boxShadow).toHaveProperty('spreadRadius');
      expect(boxShadow).toHaveProperty('color');
    });

    test('should work with object destructuring', () => {
      const boxShadow: BoxShadow = {
        offsetX: '5px',
        offsetY: '10px',
        blurRadius: '15px',
        spreadRadius: '2px',
        color: 'rgba(0, 0, 0, 0.3)',
      };

      const { offsetX, offsetY, blurRadius, spreadRadius, color } = boxShadow;

      expect(offsetX).toBe('5px');
      expect(offsetY).toBe('10px');
      expect(blurRadius).toBe('15px');
      expect(spreadRadius).toBe('2px');
      expect(color).toBe('rgba(0, 0, 0, 0.3)');
    });

    test('should allow creating array of BoxShadow objects', () => {
      const shadows: BoxShadow[] = [
        {
          offsetX: '0px',
          offsetY: '2px',
          blurRadius: '4px',
          spreadRadius: '0px',
          color: 'rgba(0, 0, 0, 0.1)',
        },
        {
          offsetX: '0px',
          offsetY: '4px',
          blurRadius: '8px',
          spreadRadius: '0px',
          color: 'rgba(0, 0, 0, 0.15)',
        },
      ];

      expect(shadows).toHaveLength(2);
      expect(shadows[0].offsetY).toBe('2px');
      expect(shadows[1].offsetY).toBe('4px');
    });

    test('should work with partial object spread', () => {
      const baseBoxShadow: BoxShadow = {
        offsetX: '0px',
        offsetY: '0px',
        blurRadius: '0px',
        spreadRadius: '0px',
        color: 'black',
      };

      const modifiedBoxShadow: BoxShadow = {
        ...baseBoxShadow,
        blurRadius: '10px',
        color: 'rgba(0, 0, 0, 0.5)',
      };

      expect(modifiedBoxShadow.offsetX).toBe('0px');
      expect(modifiedBoxShadow.offsetY).toBe('0px');
      expect(modifiedBoxShadow.blurRadius).toBe('10px');
      expect(modifiedBoxShadow.spreadRadius).toBe('0px');
      expect(modifiedBoxShadow.color).toBe('rgba(0, 0, 0, 0.5)');
    });
  });

  describe('Common Box Shadow Patterns', () => {
    test('should represent standard drop shadow', () => {
      const dropShadow: BoxShadow = {
        offsetX: '0px',
        offsetY: '4px',
        blurRadius: '6px',
        spreadRadius: '0px',
        color: 'rgba(0, 0, 0, 0.1)',
      };

      expect(dropShadow.offsetX).toBe('0px');
      expect(parseInt(dropShadow.offsetY)).toBeGreaterThan(0);
      expect(parseInt(dropShadow.blurRadius)).toBeGreaterThan(0);
    });

    test('should represent inner shadow (conceptually)', () => {
      const innerShadow: BoxShadow = {
        offsetX: '0px',
        offsetY: '2px',
        blurRadius: '4px',
        spreadRadius: '0px',
        color: 'rgba(0, 0, 0, 0.06)',
      };

      expect(innerShadow).toBeDefined();
    });

    test('should represent no shadow', () => {
      const noShadow: BoxShadow = {
        offsetX: '0px',
        offsetY: '0px',
        blurRadius: '0px',
        spreadRadius: '0px',
        color: 'transparent',
      };

      expect(noShadow.offsetX).toBe('0px');
      expect(noShadow.offsetY).toBe('0px');
      expect(noShadow.blurRadius).toBe('0px');
      expect(noShadow.spreadRadius).toBe('0px');
    });
  });
});
