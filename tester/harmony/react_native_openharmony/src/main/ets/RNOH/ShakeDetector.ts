/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import sensor from '@ohos.sensor';
import { RNOHError } from './RNOHError';

export type ShakeListener = () => void;


/**
 * @Internal
 */
export class ShakeDetector {
  static from(shakeListener: ShakeListener) {
    try {
      return new ShakeDetector(shakeListener)
    } catch (err) {
      if (err instanceof Error) {
        throw new RNOHError({
          whatHappened: "Couldn't create a ShakeDetector",
          howCanItBeFixed: ["Did you add a permission request for ohos.permission.ACCELEROMETER in module.json5::module::requestPermissions?"],
          extraData: err,
        })
      }
      throw new RNOHError({
        whatHappened: "Failed to create a RealShakeDetector",
        howCanItBeFixed: [],
        extraData: err
      });
    }
  }

  private readonly config = {
    MIN_TIME_BETWEEN_SAMPLES_NS: 20000000,
    SHAKING_WINDOW_MS: 3000,
    GRAVITY_EARTH: 9.80665,
    MIN_NUM_SHAKES: 2 * 8,
    /**
     *  Required force to constitute a rage shake. Need to multiply Earth's gravity by 1.33 because a rage
     *  shake in one direction should have more force than just the magnitude of free fall.
     */
    REQUIRED_FORCE: 9.80665 * 1.33
  }

  private mAccelerationX: number = 0;
  private mAccelerationY: number = 0;
  private mAccelerationZ: number = 0;
  private mNumShakes: number = 0;
  private mLastShakeTimestamp: number = 0;

  private internalShakeListener: (data: sensor.AccelerometerResponse) => void

  private constructor(private shakeListener: ShakeListener) {
    this.internalShakeListener = this.onSensorChanged.bind(this)
    sensor.on(sensor.SensorId.ACCELEROMETER, this.internalShakeListener, {
      interval: this.config.MIN_TIME_BETWEEN_SAMPLES_NS,
    });
  }

  onDestroy() {
    sensor.off(sensor.SensorId.ACCELEROMETER, this.internalShakeListener)
  }

  resetDetector() {
    this.reset();
  }

  private reset() {
    this.mAccelerationX = 0;
    this.mAccelerationY = 0;
    this.mAccelerationZ = 0;
    this.mNumShakes = 0;
  }

  private atLeastRequiredForce(a: number): boolean {
    return Math.abs(a) > this.config.REQUIRED_FORCE;
  }

  private recordShake(timestamp: number): void {
    this.mLastShakeTimestamp = timestamp;
    this.mNumShakes++;
  }

  private onSensorChanged(data: sensor.AccelerometerResponse) {
    const timestamp = Date.now();
    const { x, y } = data;
    const z = data.z - this.config.GRAVITY_EARTH;

    if (this.atLeastRequiredForce(x) && x * this.mAccelerationX <= 0) {
      this.recordShake(timestamp);
      this.mAccelerationX = x;
    } else if (this.atLeastRequiredForce(y) && y * this.mAccelerationY <= 0) {
      this.recordShake(timestamp);
      this.mAccelerationY = y;
    } else if (this.atLeastRequiredForce(z) && z * this.mAccelerationZ <= 0) {
      this.recordShake(timestamp);
      this.mAccelerationZ = z;
    }

    this.maybeDispatchShake(timestamp);
  }

  private maybeDispatchShake(timestamp: number) {
    if (this.mNumShakes >= this.config.MIN_NUM_SHAKES) {
      this.reset();
      this.shakeListener();
    }

    if (
      timestamp - this.mLastShakeTimestamp >
      this.config.SHAKING_WINDOW_MS
    ) {
      this.reset();
    }
  }
}
