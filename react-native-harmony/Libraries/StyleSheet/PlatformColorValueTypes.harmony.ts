import { NativePlatformColor } from "./NativePlatformColor";

export const PlatformColor = (...colors: string[]) => {
  const color = NativePlatformColor.getSystemColor(colors);
  return color;
};

export const normalizeColorObject = (color: string) => {
  return color;
};

export const processColorObject = (color: string) => {
  return color;
};
