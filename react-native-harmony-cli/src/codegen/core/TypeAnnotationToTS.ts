/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import {
  BooleanTypeAnnotation,
  DoubleTypeAnnotation,
  FloatTypeAnnotation,
  Int32TypeAnnotation,
  MixedTypeAnnotation,
  ObjectTypeAnnotation,
  PropTypeAnnotation,
  ReservedPropTypeAnnotation,
  ReservedTypeAnnotation,
  StringTypeAnnotation,
  Nullable,
  NativeModuleTypeAnnotation,
  UnionTypeAnnotationMemberType,
} from '@react-native/codegen/lib/CodegenSchema';

type StringEnumTypeAnnotation = {
  readonly type: 'StringEnumTypeAnnotation';
  readonly options: readonly string[];
};

type Int32EnumTypeAnnotation = {
  readonly type: 'Int32EnumTypeAnnotation';
  readonly options: readonly number[];
};

type ArrayTypeAnnotation = {
  readonly type: 'ArrayTypeAnnotation';
  readonly elementType: TypeAnnotation;
};

export type TypeAnnotation =
  | BooleanTypeAnnotation
  | StringTypeAnnotation
  | DoubleTypeAnnotation
  | FloatTypeAnnotation
  | Int32TypeAnnotation
  | ReservedPropTypeAnnotation
  | ReservedTypeAnnotation
  | StringEnumTypeAnnotation
  | Int32EnumTypeAnnotation
  | MixedTypeAnnotation
  | ArrayTypeAnnotation
  | NativeModuleTypeAnnotation
  | Nullable<NativeModuleTypeAnnotation>
  | ObjectTypeAnnotation<TypeAnnotation>;

export class TypeAnnotationToTS {
  constructor(private aliasPrefix: string = '') {}

  convert(typeAnnotation: TypeAnnotation | undefined): string {
    if (!typeAnnotation) {
      return 'unknown';
    }
    switch (typeAnnotation.type) {
      case 'BooleanTypeAnnotation':
        return 'boolean';
      case 'DoubleTypeAnnotation':
        return 'number';
      case 'FloatTypeAnnotation':
        return 'number';
      case 'Int32EnumTypeAnnotation':
        return typeAnnotation.options.join(' | ');
      case 'Int32TypeAnnotation':
        return 'number';
      case 'MixedTypeAnnotation':
        return 'unknown';
      case 'ReservedTypeAnnotation':
        switch (typeAnnotation.name) {
          case 'RootTag':
            return 'Tag';
          default:
            return 'unknown';
        }
      case 'ReservedPropTypeAnnotation':
        switch (typeAnnotation.name) {
          case 'ColorPrimitive':
            return 'ColorValue';
          case 'DimensionPrimitive':
            return 'unknown';
          case 'EdgeInsetsPrimitive':
            return 'unknown';
          case 'ImageRequestPrimitive':
            return 'unknown';
          case 'ImageSourcePrimitive':
            return 'unknown';
          case 'PointPrimitive':
            return 'unknown';
          default:
            return 'unknown';
        }
      case 'StringEnumTypeAnnotation':
        return typeAnnotation.options
          .map((option) => {
            if (typeof option !== 'string') {
              return option;
            }
            return wrapWithQuotes(option);
          })
          .join(' | ');
      case 'StringTypeAnnotation':
        return 'string';
      case 'FunctionTypeAnnotation':
        const { params, returnTypeAnnotation } = typeAnnotation;
        const stringifiedArgs = params
          .map(
            (param) => `${param.name}: ${this.convert(param.typeAnnotation)}`
          )
          .join(', ');
        return `(${stringifiedArgs}) => ${this.convert(returnTypeAnnotation)}`;

      case 'EnumDeclaration':
        return typeAnnotation.name;
      case 'GenericObjectTypeAnnotation':
        return 'Object';
      case 'TypeAliasTypeAnnotation':
        return this.aliasPrefix + typeAnnotation.name;
      case 'UnionTypeAnnotation':
        return this.convertUnionMemberAnnotation(typeAnnotation.memberType);
      case 'NumberTypeAnnotation':
        return 'number';
      case 'NullableTypeAnnotation':
        return `null | ${this.convert(typeAnnotation.typeAnnotation)}`;
      case 'ObjectTypeAnnotation':
        return this.convertObjectTypeAnnotation(typeAnnotation);
      case 'PromiseTypeAnnotation':
        return `Promise<${this.convert(typeAnnotation.elementType)}>`;
      case 'VoidTypeAnnotation':
        return 'void';
      case 'ArrayTypeAnnotation':
        return `${this.convert(typeAnnotation.elementType)}[]`;
      default:
        return 'unknown';
    }
  }

  private convertUnionMemberAnnotation(
    memberAnnotation: UnionTypeAnnotationMemberType
  ) {
    switch (memberAnnotation) {
      case 'NumberTypeAnnotation':
        return 'number';
      case 'ObjectTypeAnnotation':
        return 'Object';
      case 'StringTypeAnnotation':
        return 'string';
    }
  }

  private convertObjectTypeAnnotation(
    objectTypeAnnotation: ObjectTypeAnnotation<
      TypeAnnotation | Nullable<NativeModuleTypeAnnotation>
    >
  ): string {
    const props = objectTypeAnnotation.properties
      .map((prop) => {
        const maybeOptionalSymbol = prop.optional ? '?' : '';
        return `${prop.name}${maybeOptionalSymbol}: ${this.convert(
          prop.typeAnnotation
        )}`;
      })
      .join(', ');
    if (props.length === 0) {
      return '{}';
    }
    return `{${props}}`;
  }
}

export function getStringifiedTSDefaultValueFromPropTypeAnnotation(
  propTypeAnnotation: PropTypeAnnotation
): string | null {
  if ('default' in propTypeAnnotation && propTypeAnnotation.default !== null) {
    if (typeof propTypeAnnotation.default === 'string') {
      return wrapWithQuotes(propTypeAnnotation.default);
    }
    return String(propTypeAnnotation.default);
  }
  return null;
}

export function wrapWithQuotes(str: string): string {
  if (str.includes("'")) {
    return `"${str}"`;
  } else {
    return `'${str}'`;
  }
}
