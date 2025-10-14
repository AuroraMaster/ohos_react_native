import { TurboModuleRegistry } from 'react-native';
import type { TurboModule } from 'react-native/Libraries/TurboModule/RCTExport';

export interface Spec extends TurboModule {
  isScreenReaderEnabled: () => Promise<boolean>;
  isAccessibilityServiceEnabled: () => Promise<boolean>;
  isBoldTextEnabled: () => Promise<boolean>;
  announceForAccessibility(announcement: string): Promise<void>;
  setAccessibilityFocus(tag: number): Promise<void>;
}

export default TurboModuleRegistry.get<Spec>('AccessibilityInfo');
