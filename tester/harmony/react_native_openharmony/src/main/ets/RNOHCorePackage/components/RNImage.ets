/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import { ColorSegments, Descriptor, getTintColorMatrix, RNOHContext } from '../../RNOH';
import { ImageLoaderTurboModule } from '../turboModules/ImageLoaderTurboModule';
import { RNViewBase, ViewBaseProps, ViewDescriptorWrapperBase, ViewRawProps } from './RNViewBase';
import { RemoteImageLoaderError } from '../../RemoteImageLoader/RemoteImageLoaderError';
export interface ImageProps extends ViewBaseProps {
  uri: string;
  resizeMode: number;
  tintColor?: ColorSegments;
  blurRadius: number;
  defaultSource?: string;
}

interface ImageRawProps extends ViewRawProps {
  alt: string;
  resizeMethod: string;
  loadingIndicatorSource?: ImageLoadingIndicatorSource;
}

interface ImageLoadingIndicatorSource {
  uri: string;
  width: number;
  height: number;
  scale: number;
}

interface ImageOnCompleteEvent {
  width: number;
  height: number;
  componentWidth: number;
  componentHeight: number;
  loadingStatus: number;
  contentWidth: number;
  contentHeight: number;
  contentOffsetX: number;
  contentOffsetY: number;
}

interface ImageOnErrorEvent {
  componentWidth: number;
  componentHeight: number;
  message?: string;
}

export interface ImageState {}


export type ImageDescriptor = Descriptor<"Image", ImageProps, ImageState, ImageRawProps>;

class ImageDescriptorWrapper extends ViewDescriptorWrapperBase<"Image", ImageProps, ImageState, ImageRawProps> {
}

class ImageSourceHolder {
  public source: string | Resource | PixelMap

  constructor(source: string | Resource | PixelMap) {
    this.source = source
  }
}

@Component
export struct RNImage {
  ctx!: RNOHContext
  tag: number = 0
  @State descriptor: ImageDescriptor = Object() as ImageDescriptor
  @State descriptorWrapper: ImageDescriptorWrapper | undefined = undefined
  // workaround: `@State` fields cannot be unions of primitive (`string`) and object (`Resource`, `PixelMap`) types
  @State imageSource: ImageSourceHolder | undefined = undefined
  defaultColorFilter: number[] = [
    1, 0, 0, 0, 0,
    0, 1, 0, 0, 0,
    0, 0, 1, 0, 0,
    0, 0, 0, 1, 0,
  ]
  private unregisterDescriptorChangesListener?: () => void = undefined

  aboutToAppear() {
    this.descriptor = this.ctx.descriptorRegistry.getDescriptor<ImageDescriptor>(this.tag)
    this.descriptorWrapper = new ImageDescriptorWrapper(this.descriptor)
    this.onLoadStart();
    this.updateImageSource();
    this.unregisterDescriptorChangesListener = this.ctx.descriptorRegistry.subscribeToDescriptorChanges(this.tag,
      (newDescriptor) => {
        const newImageDescriptor = (newDescriptor as ImageDescriptor);
        const uriChanged = newImageDescriptor.props.uri !== this.descriptor.props.uri;
        this.descriptor = newImageDescriptor;
        this.descriptorWrapper = new ImageDescriptorWrapper(this.descriptor)
        if (uriChanged) {
          this.onLoadStart();
          this.updateImageSource();
        }
      }
    )
  }

  aboutToDisappear() {
    this.unregisterDescriptorChangesListener?.()
  }

  async updateImageSource() {
    const uri = this.descriptor.props.uri;
    if (uri.startsWith("asset://")) {
      this.imageSource = new ImageSourceHolder($rawfile(uri.replace("asset://", this.ctx.rnInstance.getAssetsDest())));
      return;
    }
    if (uri.startsWith("data:")) {
      this.imageSource = new ImageSourceHolder(uri);
      return;
    }
    const skipImageLoader = true && !this.ctx.rnInstance.isFeatureFlagEnabled("IMAGE_LOADER");
    const imageLoader = this.ctx.rnInstance.getTurboModule<ImageLoaderTurboModule>("ImageLoader");
    const queryCacheResult = await imageLoader.queryCache([uri]) as Record<string, string>;
    const isNotCached = !queryCacheResult[uri];
    if (skipImageLoader && isNotCached) {
      this.imageSource = new ImageSourceHolder(uri);
      return;
    }

    this.imageSource = undefined;
    imageLoader.getRemoteImageSource(uri).then(async (remoteImage) => {
      try {
        const imageSource = remoteImage.getImageSource();
        const frameCounter = await imageSource.getFrameCount();
        if (frameCounter === 1) {
          // use the downloaded or cached image source
          this.imageSource = new ImageSourceHolder(await imageLoader.getPixelMap(uri));
        } else {
          // an animated GIF
          this.imageSource = new ImageSourceHolder(remoteImage.getLocation())
        }
      } catch (error) {
        this.dispatchOnError(error.message);
        this.onLoadEnd()
      }
    }).catch((error: RemoteImageLoaderError) => {
      this.dispatchOnError(error.message)
      this.onLoadEnd()
    })
  }

  getLoadingPlaceholder(): string | Resource | undefined {
    const loadingPlaceholder = this.descriptor.props.defaultSource || this.descriptor.rawProps.loadingIndicatorSource?.uri;
    if (loadingPlaceholder === undefined) {
      return undefined;
    }
    if (loadingPlaceholder.startsWith("asset://")) {
      return $rawfile(loadingPlaceholder.replace("asset://", this.ctx.rnInstance.getAssetsDest()));
    }
    return loadingPlaceholder;
  }

  /*
  * enum class ImageResizeMode {
  *   Cover,
  *   Contain,
  *   Stretch,
  *   Center,
  *   Repeat
  * }
   */
  getResizeMode(resizeMode: number) {
    switch (resizeMode) {
      case 0:
        return ImageFit.Cover;
      case 1:
        return ImageFit.Contain;
      case 2:
        return ImageFit.Fill;
      case 3:
      case 4:
        return ImageFit.None;
      default:
        return ImageFit.Cover;
    }
  }

  onLoadStart() {
    this.ctx.rnInstance.emitComponentEvent(
      this.descriptor.tag,
      "loadStart",
      null
    )
  }

  onLoadEnd() {
    this.ctx.rnInstance.emitComponentEvent(
      this.descriptor.tag,
      "loadEnd",
      null
    )
  }

  onLoad(event?: ImageOnCompleteEvent) {
    if (this.imageSource !== undefined && event) {
      this.ctx.rnInstance.emitComponentEvent(this.descriptor.tag, "load", {
        width: event.width,
        height: event.height,
        uri: this.descriptor.props.uri,
      })
    }
  }

  dispatchOnError(message?: string) {
    const errorMessage = message ? message : "Failed to load the image";
    this.ctx.rnInstance.emitComponentEvent(this.descriptor.tag, "error", errorMessage)
  }

  getColorFilter(): number[] {
    if (!this.descriptor.props.tintColor || this.descriptor.props.tintColor.every((element) => element === 0)) {
      return this.defaultColorFilter;
    }

    return getTintColorMatrix(this.descriptor.props.tintColor)
  }

  build() {
    RNViewBase({
      ctx: this.ctx,
      tag: this.tag,
      controlsFocus: false
    }) {
      // NOTE: this ensures we switch the image source, even if the new source is bad.
      // ArkUI keeps displaying the last "good" image when the passed source parameter changes
      if (this.imageSource?.source) {
        Image(this.imageSource.source)
          .accessibilityText(this.descriptor.rawProps.alt)
          .alt(this.getLoadingPlaceholder())
          .interpolation(ImageInterpolation.High)
          .draggable(false)
          .width(this.descriptorWrapper?.width)
          .height(this.descriptorWrapper?.height)
          .position({
            x: 0,
            y: 0,
          })
          .autoResize(this.descriptor.rawProps.resizeMethod !== 'scale')
          .objectFit(this.getResizeMode(this.descriptor.props.resizeMode))
          .objectRepeat(this.descriptor.props.resizeMode === 4 ?
          ImageRepeat.XY : ImageRepeat.NoRepeat)
          .colorFilter(this.getColorFilter())
          .blur(this.descriptor.props.blurRadius)
          .onComplete(event => {
            this.onLoad(event)
            this.onLoadEnd()
          })
          .onError((event: ImageOnErrorEvent) => {
            if (this.imageSource !== undefined) {
              this.dispatchOnError(event.message)
              this.onLoadEnd()
            }
            this.imageSource = undefined;
          })
          .focusable(this.descriptorWrapper?.focusable ?? false)
          .hitTestBehavior(HitTestMode.None)
      }
    }
  }
}
