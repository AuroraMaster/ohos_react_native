# Usage of the C-API Component Hybrid Solution

> Note that the ArkTS component that can be used in C-API supports only leaf nodes, such as `Image`, and does not support container nodes, such as `Stack`.

## Rendering an ArkTS Component in C-API

1. Implement a custom component and use it in service code.

2. Create a custom build function and use `@Builder` to decorate it. Design a factory method in the build function to build a component based on `ComponentName`. The parameter is `ComponentBuilderContext`, which records the information required for building a component, such as `ComponentName` and `Tag`. Note that the factory method needs to be wrapped by a `Stack` component. The `position` attribute of the `Stack` component needs to be set to `(0, 0)` to ensure that the component is displayed in the correct position.

    ```TypeScript
    @Builder
    export function buildCustomComponent(ctx: ComponentBuilderContext) {
    Stack() {
      if (ctx.componentName === MarqueeView.NAME) {
        MarqueeView({
            ctx: ctx.rnComponentContext,
            tag: ctx.tag
        })
      }
    }
    .position({ x: 0, y: 0 })
    }
    ```

3. Use the `wrapBuilder` function and pass it to `RNApp` or `RNComponentContext` as a parameter. `RNComponentContext` is a required parameter of `RNSurface`.

    ```TypeScript
    const wrappedCustomRNComponentBuilder = wrapBuilder(buildCustomRNComponent);
    ···
    // Used as a parameter of RNApp.
    RNApp({
      ···
      wrappedCustomRNComponentBuilder: wrappedCustomRNComponentBuilder,
      ···
    })
    
    // Used as a parameter of RNComponentContext
    new RNComponentContext(
      RNOHContext.fromCoreContext(rnohCoreContext!, rnInstance),
      wrappedCustomRNComponentBuilder,
      wrapBuilder(buildRNComponentForTag),
      new Map(),
    )
    ```

4. When creating an `RNApp` or `RNInstance`, place the component name in the array and pass it as a parameter of `arkTsComponentNames`.
   
   ```TypeScript
   const arkTsComponentNames = [MarqueeView.NAME];

   // Used as a parameter of RNApp.
   RNApp({
     rnInstanceConfig: {
       ···
       arkTsComponentNames: arkTsComponentNames
     },
     ···
   })

   // Passed in when creating an RNInstance.
   await rnohCoreContext.createAndRegisterRNInstance({
     ···
     arkTsComponentNames: arkTsComponentNames
   });
   ```

5. Use `RNApp` or `RNSurface` to start React Native for OpenHarmony.

## Using an ArkTS Component in a C-API Component

Currently, the RNOH framework supports the mounting of an ArkTS component to the C-API component.

1. Use `createArkTSComponent` to create an ArkTS component.

   `createArkTSComponent` needs to take the following parameters:

   - `tag`: component tag
   - `componentHandle`: component handle (`facebook::react::ComponentHandle`)
   - `componentName`: component name, which must be the same as the custom ArkTS component name defined in the `buildCustomRNComponent` function of the entry point file.

   ```cpp
   auto componentInstanceFactory = rnInstance->getComponentInstanceFactory();
   auto newChildComponentInstance = componentInstanceFactory->createArkTSComponent(tag, componentHandle, componentName);
   ```

2. Call the `onChildInserted` method of the component to insert the component.

   ```cpp
   if (newChildComponentInstance != nullptr) {
     this->onChildInserted(newChildComponentInstance, index);
   }
   ```
