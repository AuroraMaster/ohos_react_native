/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import { TypeAnnotationToTS, TypeAnnotation } from './TypeAnnotationToTS';

function convert(typeAnnotation: TypeAnnotation) {
  return new TypeAnnotationToTS().convert(typeAnnotation);
}

it('should convert boolean prop correctly', () => {
  const result = convert({
    type: 'BooleanTypeAnnotation',
  });

  expect(result).toBe('boolean');
});

it('should convert string prop correctly', () => {
  const result = convert({
    type: 'StringTypeAnnotation',
  });

  expect(result).toBe('string');
});

it('should convert double prop correctly', () => {
  const result = convert({
    type: 'DoubleTypeAnnotation',
  });

  expect(result).toBe('number');
});

it('should convert float prop correctly', () => {
  const result = convert({
    type: 'FloatTypeAnnotation',
  });

  expect(result).toBe('number');
});

it('should convert object prop correctly', () => {
  const result = convert({
    type: 'ObjectTypeAnnotation',
    properties: [
      {
        typeAnnotation: { type: 'BooleanTypeAnnotation' },
        name: 'booleanTest',
        optional: false,
      },
      {
        typeAnnotation: { type: 'ObjectTypeAnnotation', properties: [] },
        name: 'objectTest',
        optional: true,
      },
    ],
  });

  expect(result).toBe('{booleanTest: boolean, objectTest?: {}}');
});

it('should convert array correctly', () => {
  const result = convert({
    type: 'ObjectTypeAnnotation',
    properties: [
      {
        typeAnnotation: {
          type: 'ArrayTypeAnnotation',
          elementType: { type: 'StringTypeAnnotation' },
        },
        name: 'arrayTest',
        optional: false,
      },
    ],
  });

  expect(result).toBe('{arrayTest: string[]}');
});
