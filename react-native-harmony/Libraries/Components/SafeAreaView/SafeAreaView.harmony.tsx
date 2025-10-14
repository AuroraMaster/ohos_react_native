import type { TurboModule } from "react-native/Libraries/TurboModule/RCTExport";
import { Dimensions, TurboModuleRegistry, View, ViewProps } from "react-native";
import { useEffect, useState } from "react";
import React from "react";

import RCTDeviceEventEmitter from "react-native/Libraries/EventEmitter/RCTDeviceEventEmitter.js";

type SafeAreaInsets = {
  top: number;
  left: number;
  right: number;
  bottom: number;
};

interface SafeAreaTurboModuleProtocol {
  getInitialInsets(): SafeAreaInsets;
}

interface Spec extends TurboModule, SafeAreaTurboModuleProtocol {}

const safeAreaTurboModule = TurboModuleRegistry.get<Spec>(
  "SafeAreaTurboModule"
)!;

const getPaddingTop = (inset: number, pageY: number) => {
  return Math.max(0, inset - (pageY < 0 ? pageY * -1 : pageY));
}

const getPaddingBottom = (insetBottom: number, insetTop: number, paddingTop: number, height: number, windowHeight: number, pageY: number, positionY: number): number => {
  // if SafeArea is not visible or outside the viewport or topped and not full height
  if (height === 0 || (pageY === 0 && height < windowHeight)) {
    return Math.max(0, insetBottom - (Math.round(windowHeight) - Math.round(height)));
  }

  // if SafeAreaView is not topped and not full height and not visible
  if (pageY < windowHeight && pageY > height && positionY === 0) {
    return 0;
  };

  
  // if SafeAreaView is topped, check for full height
  if (Math.round(height) >= Math.round(windowHeight) && pageY === 0) {
    // if SafeAreaView is full height and at the top without any offset
    return positionY === 0 ? insetBottom : 0;
  }
  
  // if SafeAreaView is topped with margin and not full height
  if (height < windowHeight && positionY === 0 && pageY <= insetTop) {
    return Math.max(0, insetBottom - (windowHeight - (height + pageY)));
  }

  if (height < windowHeight && pageY < windowHeight && pageY > 0 && positionY > 0) {
    return Math.max(0, insetBottom - (windowHeight - pageY));
  }

  // if SafeAreaView nested and outside of the current viewport - for example in scroll view
  if (height < windowHeight && pageY > 0 && pageY > windowHeight && positionY >= 0) {
    return 0;
  }
  // if SafeAreaView is nested outside of the viewport - but on absolute position
  if (height < windowHeight && pageY > 0 && pageY > windowHeight) {
    return insetBottom;
  }
  
  // Default case handling scenarios not captured above
  return Math.max(0, insetBottom - (windowHeight - height + paddingTop));
}

export default React.forwardRef<View, ViewProps>(
  ({ children, style, ...otherProps }, ref) => {

    const safeAreaViewRef = React.useRef<View>(null);

    const [topInset, setTopInset] = useState(safeAreaTurboModule.getInitialInsets().top);
    const [leftInset, setLeftInset] = useState(safeAreaTurboModule.getInitialInsets().left);
    const [rightInset, setRightInset] = useState(safeAreaTurboModule.getInitialInsets().right);
    const [bottomInset, setBottomInset] = useState(safeAreaTurboModule.getInitialInsets().bottom);

    const [measurement, setMeasurement] = useState({ x: 0, y: 0, width: 0, height: 0, pageX: 0, pageY: -1 });
    const [layout, setLayout] = useState({ x: 0, y: 0, width: 0, height: 0 });

    const measureView = () => {
      safeAreaViewRef?.current?.measure((x, y, width, height, pageX, pageY) => {
        setMeasurement({ x, y, width, height, pageX, pageY });
      });
    }
  
    useEffect(
      function subscribeToSafeAreaChanges() {

        const subscription = (RCTDeviceEventEmitter as any).addListener(
          "SAFE_AREA_INSETS_CHANGE",
          (insets: SafeAreaInsets) => {
            setTopInset(insets.top);
            setBottomInset(insets.bottom);
            setLeftInset(insets.left);
            setRightInset(insets.right);
          }
        );
        return () => {
          subscription.remove();
        };
      },
      [setTopInset, setLeftInset, setRightInset, setBottomInset, measurement.pageY]
    );

    useEffect(() => {
      measureView();
    }, []);

    const isPaddingBottomExplicit = style?.paddingBottom !== undefined;
    const isPaddingTopExplicit = style?.paddingTop !== undefined;
    const isPaddingLeftExplicit = style?.paddingLeft !== undefined;
    const isPaddingRightExplicit = style?.paddingRight !== undefined;

    const windowHeight = Dimensions.get('window').height;
    const paddingTop = getPaddingTop(topInset, measurement.pageY);
    const paddingBottom = getPaddingBottom(bottomInset, topInset, paddingTop, measurement.height, windowHeight, measurement.pageY, layout.y);

    return (
      <View
        ref={safeAreaViewRef}
        style={[
          style,
          {
            paddingTop: isPaddingTopExplicit ? style.paddingBottom : paddingTop,
            paddingLeft: isPaddingLeftExplicit ? style.paddingLeft : leftInset,
            paddingRight: isPaddingRightExplicit ? style.paddingRight : rightInset,
            paddingBottom: isPaddingBottomExplicit ? style.paddingBottom : paddingBottom,
          },
        ]}
        onLayout={(event) => {
          setLayout(event.nativeEvent.layout);
          measureView();
          otherProps?.onLayout && otherProps.onLayout(event);
        }}
        {...otherProps}
      >
        {children}
      </View>
    );
  }
);
