/**
 * Unit tests for PredictionResult.ts
 */
import { PredictionResult } from '../src/Framework/Popup/PredictionResult';

describe('PredictionResult Interface', () => {
  describe('Interface structure', () => {
    test('should allow creating object with prediction 0', () => {
      const result: PredictionResult = {
        prediction: 0,
        probability: 0.25,
      };

      expect(result.prediction).toBe(0);
      expect(result.probability).toBe(0.25);
    });

    test('should allow creating object with prediction 1', () => {
      const result: PredictionResult = {
        prediction: 1,
        probability: 0.85,
      };

      expect(result.prediction).toBe(1);
      expect(result.probability).toBe(0.85);
    });

    test('should have required prediction property', () => {
      const result: PredictionResult = {
        prediction: 0,
        probability: 0.5,
      };

      expect(result).toHaveProperty('prediction');
      expect(typeof result.prediction).toBe('number');
    });

    test('should have required probability property', () => {
      const result: PredictionResult = {
        prediction: 1,
        probability: 0.9,
      };

      expect(result).toHaveProperty('probability');
      expect(typeof result.probability).toBe('number');
    });
  });

  describe('Prediction values', () => {
    test('should accept prediction value 0', () => {
      const result: PredictionResult = {
        prediction: 0,
        probability: 0.3,
      };

      expect(result.prediction).toBe(0);
    });

    test('should accept prediction value 1', () => {
      const result: PredictionResult = {
        prediction: 1,
        probability: 0.7,
      };

      expect(result.prediction).toBe(1);
    });

    test('should work in conditional logic', () => {
      const result: PredictionResult = {
        prediction: 1,
        probability: 0.95,
      };

      const isPositive = result.prediction === 1;
      expect(isPositive).toBe(true);
    });

    test('should distinguish between 0 and 1', () => {
      const result0: PredictionResult = { prediction: 0, probability: 0.1 };
      const result1: PredictionResult = { prediction: 1, probability: 0.9 };

      expect(result0.prediction).not.toBe(result1.prediction);
    });
  });

  describe('Probability values', () => {
    test('should accept probability 0.0', () => {
      const result: PredictionResult = {
        prediction: 0,
        probability: 0.0,
      };

      expect(result.probability).toBe(0.0);
    });

    test('should accept probability 1.0', () => {
      const result: PredictionResult = {
        prediction: 1,
        probability: 1.0,
      };

      expect(result.probability).toBe(1.0);
    });

    test('should accept probability 0.5', () => {
      const result: PredictionResult = {
        prediction: 0,
        probability: 0.5,
      };

      expect(result.probability).toBe(0.5);
    });

    test('should handle decimal probabilities', () => {
      const result: PredictionResult = {
        prediction: 1,
        probability: 0.7654321,
      };

      expect(result.probability).toBeCloseTo(0.7654321);
    });
  });

  describe('Practical usage', () => {
    test('should work with array of results', () => {
      const results: PredictionResult[] = [
        { prediction: 0, probability: 0.2 },
        { prediction: 1, probability: 0.8 },
        { prediction: 1, probability: 0.9 },
      ];

      expect(results.length).toBe(3);
      expect(results[0].prediction).toBe(0);
      expect(results[1].prediction).toBe(1);
    });

    test('should work in function parameters', () => {
      const isConfident = (result: PredictionResult): boolean => {
        return result.probability > 0.8;
      };

      const highConfidence: PredictionResult = { prediction: 1, probability: 0.95 };
      const lowConfidence: PredictionResult = { prediction: 0, probability: 0.3 };

      expect(isConfident(highConfidence)).toBe(true);
      expect(isConfident(lowConfidence)).toBe(false);
    });

    test('should work in function return values', () => {
      const createResult = (pred: 0 | 1, prob: number): PredictionResult => {
        return { prediction: pred, probability: prob };
      };

      const result = createResult(1, 0.85);
      expect(result.prediction).toBe(1);
      expect(result.probability).toBe(0.85);
    });

    test('should support filtering operations', () => {
      const results: PredictionResult[] = [
        { prediction: 0, probability: 0.3 },
        { prediction: 1, probability: 0.7 },
        { prediction: 1, probability: 0.9 },
        { prediction: 0, probability: 0.4 },
      ];

      const positives = results.filter(r => r.prediction === 1);
      expect(positives.length).toBe(2);

      const confident = results.filter(r => r.probability > 0.5);
      expect(confident.length).toBe(2);
    });

    test('should support sorting by probability', () => {
      const results: PredictionResult[] = [
        { prediction: 0, probability: 0.3 },
        { prediction: 1, probability: 0.9 },
        { prediction: 1, probability: 0.7 },
      ];

      results.sort((a, b) => b.probability - a.probability);

      expect(results[0].probability).toBe(0.9);
      expect(results[1].probability).toBe(0.7);
      expect(results[2].probability).toBe(0.3);
    });
  });

  describe('Edge cases', () => {
    test('should handle minimum probability', () => {
      const result: PredictionResult = {
        prediction: 0,
        probability: 0,
      };

      expect(result.probability).toBe(0);
    });

    test('should handle maximum probability', () => {
      const result: PredictionResult = {
        prediction: 1,
        probability: 1,
      };

      expect(result.probability).toBe(1);
    });

    test('should handle very small probabilities', () => {
      const result: PredictionResult = {
        prediction: 0,
        probability: 0.0001,
      };

      expect(result.probability).toBeLessThan(0.001);
    });

    test('should handle very high probabilities', () => {
      const result: PredictionResult = {
        prediction: 1,
        probability: 0.9999,
      };

      expect(result.probability).toBeGreaterThan(0.999);
    });
  });

  describe('Type safety', () => {
    test('should maintain correct types', () => {
      const result: PredictionResult = {
        prediction: 1,
        probability: 0.8,
      };

      expect(typeof result.prediction).toBe('number');
      expect(typeof result.probability).toBe('number');
    });

    test('should work with destructuring', () => {
      const result: PredictionResult = {
        prediction: 1,
        probability: 0.75,
      };

      const { prediction, probability } = result;

      expect(prediction).toBe(1);
      expect(probability).toBe(0.75);
    });

    test('should work with object spread', () => {
      const base: PredictionResult = {
        prediction: 0,
        probability: 0.5,
      };

      const modified = { ...base, probability: 0.8 };

      expect(modified.prediction).toBe(0);
      expect(modified.probability).toBe(0.8);
    });
  });

  describe('Real-world scenarios', () => {
    test('should represent binary classification result', () => {
      const positive: PredictionResult = {
        prediction: 1,
        probability: 0.92,
      };

      expect(positive.prediction).toBe(1);
      expect(positive.probability).toBeGreaterThan(0.5);
    });

    test('should represent negative classification with low confidence', () => {
      const negative: PredictionResult = {
        prediction: 0,
        probability: 0.35,
      };

      expect(negative.prediction).toBe(0);
      expect(negative.probability).toBeLessThan(0.5);
    });

    test('should allow threshold-based decision making', () => {
      const makeDecision = (result: PredictionResult, threshold: number): string => {
        if (result.probability >= threshold) {
          return result.prediction === 1 ? 'accept' : 'reject';
        }
        return 'uncertain';
      };

      const result1: PredictionResult = { prediction: 1, probability: 0.9 };
      const result2: PredictionResult = { prediction: 0, probability: 0.4 };

      expect(makeDecision(result1, 0.8)).toBe('accept');
      expect(makeDecision(result2, 0.8)).toBe('uncertain');
    });
  });
});
