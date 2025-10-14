// This implementation is based on iOS logic from file - react-native/Libraries/Vibration/Vibration.js
import NativeVibration from 'react-native/Libraries/Vibration/NativeVibration'; // RNOH: patch

/**
 * Vibration API
 *
 * See https://reactnative.dev/docs/vibration
 */

let _vibrating: boolean = false;
let _id: number = 0; // _id is necessary to prevent race condition.
const _default_vibration_length = 400;


function vibrateByPattern(pattern: Array<number>, repeat: boolean = false) {
  if (_vibrating) {
    return;
  }
  _vibrating = true;
  if (pattern.length === 0) {
    _vibrating = false;
    return;
  }
  vibrateScheduler(++_id, pattern, repeat, 0);
}

function vibrateScheduler(
  id: number,
  pattern: Array<number>,
  repeat: boolean,
  nextIndex: number,
  shouldVibrate: boolean = false, // first value in pattern is delay
) {
  if (!_vibrating || id !== _id) {
    return;
  }
  if (shouldVibrate && nextIndex < pattern.length) {
    NativeVibration.vibrate(pattern[nextIndex]);
  }
  if (nextIndex >= pattern.length) {
    if (repeat) {
      // $FlowFixMe[reassign-const]
      nextIndex = 0;
      shouldVibrate = false;
    } else {
      _vibrating = false;
      return;
    }
  }
  setTimeout(
    () => vibrateScheduler(id, pattern, repeat, nextIndex + 1, !shouldVibrate),
    pattern[nextIndex],
  );
}

const Vibration = {
  /**
   * Trigger a vibration with specified `pattern`.
   *
   * See https://reactnative.dev/docs/vibration#vibrate
   */
  vibrate: function (
    pattern: number | Array<number> = _default_vibration_length,
    repeat: boolean = false,
  ) {
    if (_vibrating) {
      return;
    }
    if (typeof pattern === 'number') {
      NativeVibration.vibrate(pattern);
    } else if (Array.isArray(pattern)) {
      vibrateByPattern(pattern, repeat);
    } else {
      throw new Error('Vibration pattern should be a number or array');
    }
  },
  /**
   * Stop vibration
   *
   * See https://reactnative.dev/docs/vibration#cancel
   */
  cancel: function () {
    _vibrating = false;
    NativeVibration.cancel();
  },
};

module.exports = Vibration;
