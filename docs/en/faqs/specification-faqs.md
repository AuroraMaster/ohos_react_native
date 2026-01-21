# Specification FAQs

### React Native Upgraded to 0.72.5

Check [RN Upgrade Changes](../rn-upgrade-changes.md) for more details. This document lists the main changes from React Native 0.59 to React Native 0.72.5. You can change your code and upgrade React Native based on this document.

### How to limit the font scaling factor for RN aging
- Before API13, this function was not supported.
- The application configures the fontSizeMaxScale field to limit the font scaling multiple. The default project has no configuration.
- After configuration, if you adjust the system font size or display size, the RN page component will only be enlarged to a multiple of the current configuration, and will no longer be enlarged beyond this multiple.