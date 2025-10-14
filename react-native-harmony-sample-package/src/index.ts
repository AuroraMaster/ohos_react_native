/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */


export * from "./SampleComponent";
export * from "./GeneratedSampleComponentArkTS";
export * from "./GeneratedSampleComponentCAPI";
export * from "./SimpleText";
export * from "./CodegenLibSampleComponent";
import NativeContainerView from "./specs/v1/NativeContainerView";
import NativeSampleTurboModule from "./NativeSampleTurboModule";
import NativeGeneratedSampleTurboModule from "./specs/v2/NativeGeneratedSampleTurboModule";
import NativeSampleWorkerTurboModule from "./specs/v2/NativeSampleWorkerTurboModule";
import NativeCodegenLibSampleTurboModule from "./specs/codegen-lib/NativeCodegenLibSampleModule";
import NativeBindSheetView from "./specs/v1/NativeBindSheetView"

export const SampleTurboModule = NativeSampleTurboModule;
export const GeneratedSampleTurboModule = NativeGeneratedSampleTurboModule;
export const SampleWorkerTurboModule = NativeSampleWorkerTurboModule;
export const CodegenLibSampleTurboModule = NativeCodegenLibSampleTurboModule;
export const ContainerView = NativeContainerView;
export const BindSheetView = NativeBindSheetView;