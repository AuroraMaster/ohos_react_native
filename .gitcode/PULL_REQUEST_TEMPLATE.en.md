## Change Type

Please check `√` next to the corresponding type, **only one option can be selected**:

- [ ] feat (New feature, corresponds to Changelog Added)
- [ ] fix (Bug fix, corresponds to Changelog Fixed)
- [ ] docs (Documentation change, corresponds to Changelog Documentation)
- [ ] refactor (Code refactoring, corresponds to Changelog Changed)
- [ ] perf (Performance optimization, corresponds to Changelog Performance)
- [ ] test (Test related, will not be included in Changelog)
- [ ] chore (Build/Toolchain/Dependency, will not be included in Changelog)

## Change Description

- Briefly describe the specific content of this change and the problem it solves
- If this is a **breaking change**, please provide detailed information on:
  - Impact scope (which APIs/configurations/features are modified)
  - User migration guide (steps to replace old version with new version)

## Test Steps

<!--
How to test this PR. For example:
1. Open the Tester project
2. Execute npm run start command
...
-->

## Test Status

- [ ] Passed local comprehensive testing (unit tests/integration tests)
- [ ] Passed CI automatic build and testing
- [ ] Added/updated corresponding test cases
- [ ] Verified functionality in target environment (e.g., phone/tablet)

## Pre-merge Self-Check

<!--
Please conduct self inspection according to the following checklist before merging
After completing self check, fill in "x" in [], For example:
- [x] Does not involve incompatible changes; if involved, has been reviewed accordingly.
-->
- [ ] Does not involve incompatible changes; if involved, has been reviewed accordingly.
- [ ] Does not impact performance, or performance testing has been conducted without degradation.
- [ ] Complies with the relevant coding standards.
- [ ] No sensitive information leakage, such as passwords, keys, etc.
- [ ] Does not involve documentation updates, or documentation has been updated.
- [ ] Meets testability requirements with necessary self-test cases, appropriate logging, or trace information added.
- [ ] No illegal file inclusions exist, such as images or code.
