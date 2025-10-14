import { TurboModule, TurboModuleRegistry } from "react-native";

interface Spec extends TurboModule {
  getSystemColor: () => string | null;
}

export const NativePlatformColor =
  TurboModuleRegistry.getEnforcing<Spec>("PlatformColor");
