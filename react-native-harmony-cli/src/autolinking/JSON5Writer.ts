/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import JSON5 from 'json5';

/**
 * A utility class for modifying JSON5 content while preserving comments and formatting.
 * Uses json5-writer's AST-based transformation to maintain the original structure.
 */
export class JSON5Writer {
  /**
   * Updates the dependencies in a JSON5 string while preserving comments and formatting.
   *
   * @param content - The original JSON5 content
   * @param newDependencies - The new dependencies object to set
   * @returns The modified JSON5 content with preserved comments
   */
  static updateDependencies(content: string, newDependencies: Record<string, string>): string {
    const json5Writer = require('json5-writer');

    // Parse the original content to get all fields
    const parsed = JSON5.parse(content);

    // Create the new object with updated dependencies
    const newObject = {
      ...parsed,
      dependencies: newDependencies,
    };

    // Load the original content into json5-writer to preserve comments
    const writer = json5Writer.load(content);

    // Write the new object, which will preserve comments
    writer.write(newObject);

    // Return the modified source with JSON5-style formatting
    // Use double quotes for both keys and values, and trailing commas
    // quoteKeys: true ensures keys with special characters (like @, /, -) are properly quoted
    return writer.toSource({ quote: 'double', quoteKeys: true, trailingComma: true }) + '\n';
  }

  /**
   * Updates dependencies with fallback to standard JSON5 stringify if preservation fails.
   * This ensures backward compatibility.
   *
   * @param content - The original JSON5 content
   * @param newDependencies - The new dependencies object to set
   * @returns The modified JSON5 content
   */
  static updateDependenciesWithFallback(
    content: string,
    newDependencies: Record<string, string>
  ): string {
    try {
      return this.updateDependencies(content, newDependencies);
    } catch (error) {
      // Fallback to JSON5 stringify if AST transformation fails
      const parsed = JSON5.parse(content);
      const result = {
        ...parsed,
        dependencies: newDependencies,
      };
      return JSON5.stringify(result, { space: 2, quote: '"' }) + '\n';
    }
  }
}
