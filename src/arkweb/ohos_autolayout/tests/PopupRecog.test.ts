
import { PopupRecog } from '../src/Framework/Popup/PopupRecog';
import { PopupInfo } from '../src/Framework/Popup/PopupInfo';
import { PredictionResult } from '../src/Framework/Popup/PredictionResult';

describe('PopupRecog', () => {
  describe('preprocessFeatures', () => {
    it('should correctly preprocess raw features', () => {
      const rawFeatures: PopupInfo = {
        root_position: 'fixed',
        mask_position: 'absolute',
        has_mask: true,
        root_is_visiable: true,
        has_close_button: false,
        root_screen_area_ratio: 50.5,
        mask_area_ratio: 99.9,
      } as PopupInfo;

      const processed = PopupRecog.preprocessFeatures(rawFeatures);

      expect(processed.root_position).toBe(4);
      expect(processed.mask_position).toBe(5);
      expect(processed.has_mask).toBe(1);
      expect(processed.root_is_visiable).toBe(1);
      expect(processed.has_close_button).toBe(0);
      expect(processed.root_screen_area_ratio).toBe(50.5);
      expect(processed.mask_area_ratio).toBe(99.9);
    });

    it('should handle unknown position values', () => {
      const rawFeatures: PopupInfo = {
        root_position: 'unknown',
        mask_position: 'invalid',
      } as PopupInfo;

      const processed = PopupRecog.preprocessFeatures(rawFeatures);

      expect(processed.root_position).toBeNaN();
      expect(processed.mask_position).toBeNaN();
    });
  });

  describe('predictIsPopup', () => {
    it('should return a prediction and probability', () => {
      const rawFeatures: PopupInfo = {
        root_position: 'fixed',
        mask_position: 'fixed',
        has_mask: true,
        root_is_visiable: true,
        has_close_button: true,
        root_screen_area_ratio: 80,
        mask_area_ratio: 100,
      } as PopupInfo;

      const result: PredictionResult = PopupRecog.predictIsPopup(rawFeatures);

      expect(result).toHaveProperty('prediction');
      expect(result).toHaveProperty('probability');
      expect(typeof result.prediction).toBe('number');
      expect(typeof result.probability).toBe('number');
    });

    it('should predict 1 for a likely popup', () => {
        const rawFeatures: PopupInfo = {
            root_position: 'fixed',
            has_mask: true,
            root_screen_area_ratio: 90,
            root_is_visiable: true,
            has_close_button: true,
            mask_area_ratio: 100,
            mask_position: 'fixed',
          } as PopupInfo;
    
          const result = PopupRecog.predictIsPopup(rawFeatures);
          // This is a heuristic guess based on the model's likely behavior.
          // The exact probability depends on the model parameters.
          expect(result.prediction).toBe(1);
          expect(result.probability).toBeGreaterThan(0.5);
    });

    it('should predict 0 for a non-popup', () => {
        const rawFeatures: PopupInfo = {
            root_position: 'static',
            has_mask: false,
            root_screen_area_ratio: 10,
            root_is_visiable: true,
            has_close_button: false,
            mask_area_ratio: 0,
            mask_position: 'static',
          } as PopupInfo;
    
          const result = PopupRecog.predictIsPopup(rawFeatures);
          expect(result.prediction).toBe(0);
          expect(result.probability).toBeLessThan(0.5);
    });

    it('should return prediction 0 if a feature is NaN', () => {
      const rawFeatures: PopupInfo = {
        root_position: 'invalid-position', // This will become NaN
        has_mask: true,
        root_screen_area_ratio: 80,
        root_is_visiable: true,
        has_close_button: true,
        mask_area_ratio: 100,
        mask_position: 'fixed',
      } as PopupInfo;

      const result = PopupRecog.predictIsPopup(rawFeatures);
      expect(result.prediction).toBe(0);
      expect(result.probability).toBe(0);
    });
  });
});
