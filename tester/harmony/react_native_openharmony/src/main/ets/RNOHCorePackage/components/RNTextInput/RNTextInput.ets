/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import { convertColorSegmentsToString, Descriptor, RNOHContext } from '../../../RNOH';
import { RNViewBase } from '../RNViewBase';
import {
  Selection,
  TextInputEvent,
  TextInputKeyEvent,
  TextInputProps,
  TextInputRawProps,
  TextInputState
} from './types';
import { TextInputDescriptorWrapper } from './TextInputDescriptorWrapper';
import { stringToFontStyle } from '../RNText/conversions';
import { RNOHEventEmitRequestHandlerName, RNViewManager } from '../..';

export type TextInputDescriptor = Descriptor<"TextInput", TextInputProps, TextInputState, TextInputRawProps>

function getCaretStyle(caretHidden: boolean | undefined): CaretStyle | undefined {
  return caretHidden ? { width: 0 } : undefined;
}

interface SetTextAndSelectionCommandCallback {
  mostRecentEventCount: number,
  value?: string,
  start: number,
  end: number,
}

function parseSetTextAndSelectionCommandCallback(args: Object[]): SetTextAndSelectionCommandCallback {
  return {
    mostRecentEventCount: args[0] as number,
    value: args[1] as string,
    start: args[2] as number,
    end: args[3] as number,
  }
}

function stringToAlignment(alignment?: string) {
  switch (alignment) {
    case "center":
      return TextAlign.Center
    case "right":
      return TextAlign.End
    case "justified":
      return TextAlign.JUSTIFY
    default:
      return TextAlign.Start
  }
}

function stringToEnterKeyType(enterKeyType?: string): EnterKeyType {
  switch (enterKeyType) {
    case "done":
      return EnterKeyType.Done
    case "go":
      return EnterKeyType.Go
    case "next":
      return EnterKeyType.Next
    case "search":
      return EnterKeyType.Search
    case "send":
      return EnterKeyType.Send
    default:
      return EnterKeyType.Done // Default value on ArkUI. If we would set undefined, then ArkUI would keep the value that was set earlier.
  }
}

function getInputType(keyboardType?: string, secureTextEntry?: boolean): InputType {
  if (secureTextEntry) {
    return InputType.Password;
  }
  switch (keyboardType) {
    case "numberPad":
    case "numeric":
      return InputType.Number;
    case "decimalPad":
      return InputType.NUMBER_DECIMAL;
    case "phonePad":
      return InputType.PhoneNumber;
    case "emailAddress":
      return InputType.Email;
    default:
      return InputType.Normal
  }
}

function getTextAreaType(keyboardType?: string): TextAreaType {
  switch (keyboardType) {
    case "numberPad":
    case "numeric":
    case "decimalPad":
      return TextAreaType.NUMBER
    case "phonePad":
      return TextAreaType.PHONE_NUMBER
    case "emailAddress":
      return TextAreaType.EMAIL
    default:
      return TextAreaType.NORMAL
  }
}

interface CancelButtonOptions {
  style?: CancelButtonStyle;
  icon?: IconOptions;
};

@Component
export struct RNTextInput {
  ctx!: RNOHContext
  tag: number = 0
  @State descriptor: TextInputDescriptor = Object() as TextInputDescriptor
  @State private descriptorWrapper: TextInputDescriptorWrapper | undefined = undefined
  inputController: TextInputController = new TextInputController();
  areaController: TextAreaController = new TextAreaController();
  @State value: string = '';
  @State isEditing: boolean = false;
  private cleanUpCallbacks: (() => void)[] = []
  private shouldIgnoreNextValueChange: boolean = false
  private componentManager!: RNViewManager
  private nativeEventCount: number = 0
  private textWasPastedOrCut: boolean = false
  private selection: Selection = { start: 0, end: 0 }
  private valueChanged: boolean = false

  createTextInputEvent(): TextInputEvent {
    const event: TextInputEvent = {
      text: this.value,
      eventCount: this.nativeEventCount,
      selectionRange: {
        location: this.selection.start,
        length: this.selection.end - this.selection.start,
      }
    };
    return event;
  }

  createTextInputChangeEvent(): TextInputEvent {
    this.nativeEventCount++
    return this.createTextInputEvent()
  }

  createTextInputKeyEvent(key: string): TextInputKeyEvent {
    const event: TextInputKeyEvent = {
      key,
      eventCount: ++this.nativeEventCount,
    };
    return event;
  }

  handleChangeEvent(value: string) {
    this.value = value
    if (this.shouldIgnoreNextValueChange) {
      this.shouldIgnoreNextValueChange = false;
    }
    else {
      this.valueChanged = true;
    }
  }

  onFocusEvent() {
    if (this.descriptorWrapper?.clearTextOnFocus) {
      this.value = "";
      this.shouldIgnoreNextValueChange = true;
    }
    if (this.descriptorWrapper?.selectTextOnFocus) {
      this.getController().setTextSelection(0, this.value.length);
    }
    this.ctx.rnInstance.emitComponentEvent(this.descriptor.tag, "onFocus", this.createTextInputEvent());
  }

  onBlurEvent() {
    this.ctx.rnInstance.emitComponentEvent(this.descriptor.tag, "onEndEditing", this.createTextInputEvent());
    this.ctx.rnInstance.emitComponentEvent(this.descriptor.tag, "onBlur", this.createTextInputEvent());
  }

  canUpdateWithEventCount(eventCount: number | undefined): boolean {
    return eventCount !== undefined && eventCount >= this.nativeEventCount;
  }

  maybeSetValue(newValue: string, mostRecentEventCount?: number): void {
    if (this.canUpdateWithEventCount(mostRecentEventCount)) {
      this.value = newValue;
    }
  }

  getController(): TextInputController | TextAreaController {
    if (this.descriptor.props?.multiline) {
      return this.areaController;
    }
    return this.inputController;
  }

  onDescriptorChange(newDescriptor: TextInputDescriptor) {
    // only set the text value from props if it changed since the last time
    if (this.descriptor.props?.text !== newDescriptor.props.text) {
      this.maybeSetValue(newDescriptor.props.text ?? "", newDescriptor.rawProps?.mostRecentEventCount);
    }
    this.descriptor = newDescriptor;
    this.descriptorWrapper = new TextInputDescriptorWrapper(this.descriptor);
    if (this.componentManager) {
      this.componentManager?.setIsEnabled(newDescriptor.props.editable ?? true);
    }
  }

  aboutToAppear() {
    const newDescriptor = this.ctx.descriptorRegistry.getDescriptor<TextInputDescriptor>(this.tag);
    this.onDescriptorChange(newDescriptor);
    this.cleanUpCallbacks.push(this.ctx.descriptorRegistry.subscribeToDescriptorChanges(this.tag,
      (newDescriptor) => {
        this.onDescriptorChange(newDescriptor as TextInputDescriptor);
      }
    ));
    this.componentManager = new RNViewManager(this.tag, this.ctx);
    this.componentManager.setIsEnabled(newDescriptor.props.editable ?? true);
    this.cleanUpCallbacks.push(this.ctx.componentManagerRegistry.registerComponentManager(this.tag, this.componentManager));
    this.cleanUpCallbacks.push(this.ctx.componentCommandReceiver.registerCommandCallback(
      this.tag,
      (command, args: Object[]) => {
        if (command === 'blur') {
          this.getController().stopEditing();
        } else if (command === 'setTextAndSelection') {
          const parsedArgs = parseSetTextAndSelectionCommandCallback(args);
          if (parsedArgs.value !== undefined) {
            this.maybeSetValue(parsedArgs.value, parsedArgs.mostRecentEventCount);
          }
          if (parsedArgs.start !== undefined && parsedArgs.end !== undefined) {
            this.getController().setTextSelection(parsedArgs.start, parsedArgs.end);
          }
        } else if (command === 'focus') {
          focusControl.requestFocus(`TextInput@${this.tag}`)
        }
      }));
  }

  aboutToDisappear() {
    this.cleanUpCallbacks.forEach(cleanUp => cleanUp());
  }

  onSubmitEditing(): void {
    this.ctx.rnInstance.emitComponentEvent(this.descriptor.tag, "onSubmitEditing", this.createTextInputEvent());
  }

  maybeFocusOnAppear(): void {
    if (this.descriptorWrapper?.autoFocus && !this.descriptor.state.wasAlreadyRendered) {
      focusControl.requestFocus(`TextInput@${this.tag}`);
    }
    this.descriptor.state.wasAlreadyRendered = true
  }

  getCancelButton(): CancelButtonOptions {
    const clearButtonMode = this.descriptorWrapper?.clearButtonMode;
    switch (clearButtonMode) {
      case 'always':
        return { style: CancelButtonStyle.INPUT };
      case 'while-editing':
        return { style: this.isEditing ? CancelButtonStyle.INPUT : CancelButtonStyle.INVISIBLE };
      case 'unless-editing':
        return { style: this.isEditing ? CancelButtonStyle.INVISIBLE : CancelButtonStyle.INPUT };
      default:
        return { style: CancelButtonStyle.INVISIBLE };
    }
  }

  onPaste(): void {
    this.textWasPastedOrCut = true;
  }

  dispatchKeyEvent(currentSelection: Selection): void {
    if (this.textWasPastedOrCut) {
      this.textWasPastedOrCut = false;
    } else if (this.valueChanged) {
      const noPreviousSelection = this.selection.start === this.selection.end;
      const cursorDidNotMove = currentSelection.start === this.selection.start;
      const cursorMovedBackwardsOrAtBeginningOfInput = (currentSelection.start < this.selection.start) || currentSelection.start <= 0;
      if (!cursorMovedBackwardsOrAtBeginningOfInput && (noPreviousSelection || !cursorDidNotMove)) {
        /**
         * Currently the onTextSelectionChange callback is delivered before onCut
         * making it impossible to determine whether text was cut or [backspace] was clicked.
         * Because of that we do not emit the backspace event.
         * */
        const key = this.value.charAt(currentSelection.start - 1);
        this.ctx.rnInstance.emitComponentEvent(this.descriptor.tag, "onKeyPress", this.createTextInputKeyEvent(key));
      }
    }
  }

  dispatchChangeEvent(): void {
    if (this.valueChanged) {
      this.valueChanged = false;
      this.ctx.rnInstance.emitComponentEvent(this.descriptor.tag, RNOHEventEmitRequestHandlerName.TextInputChange, this.createTextInputChangeEvent());
    }
  }

  textSelectionChange(currentSelectionStart: number, currentSelectionEnd: number): void {
    /**
     * New values of selection are needed to determine what key was pressed, so this.dispatchKeyEvent needs to be
     * invoked after this callback fires. OnChange callback fires earlier than OnTextSelectionChange, so all this logic
     * needs to be handled here rather than in this.handleChangeEvent. According to RN the keyPress event should fire
     * before changeEvent, so dispatching change event needs to be handled here as well.
     * */
    this.dispatchKeyEvent({ start: currentSelectionStart, end: currentSelectionEnd });
    this.selection = { start: currentSelectionStart, end: currentSelectionEnd };
    this.dispatchChangeEvent();
    this.ctx.rnInstance.emitComponentEvent(this.descriptor.tag, "onSelectionChange", this.createTextInputEvent());
  }

  build() {
    RNViewBase({ ctx: this.ctx, tag: this.tag, componentManager: this.componentManager, controlsFocus: false }) {
      if (this.descriptor.props.multiline) {
        TextArea({ controller: this.areaController, placeholder: this.descriptor.props.placeholder, text: this.value })
          .fontColor(this.descriptorWrapper?.fontColor)
          .fontFamily(this.descriptorWrapper?.fontFamily)
          .fontSize(this.descriptorWrapper?.fontSize)
          .fontWeight(this.descriptorWrapper?.fontWeight)
          .fontStyle(stringToFontStyle(this.descriptorWrapper?.fontStyle))
          .onChange((value) => this.handleChangeEvent(value))
          .enabled(this.descriptor.props.editable ?? true)
          .maxLength(this.descriptor.rawProps.maxLength)
          .placeholderColor(convertColorSegmentsToString(this.descriptor.props.placeholderTextColor))
          .placeholderFont({
            size: this.descriptorWrapper?.fontSize,
            family: this.descriptorWrapper?.fontFamily ?? "HarmonyOS Sans",
            weight: this.descriptorWrapper?.fontWeight ?? FontWeight.Normal,
            style: stringToFontStyle(this.descriptorWrapper?.fontStyle)
          })
          .textAlign(stringToAlignment(this.descriptor.props.textAlign))/*
           * We set the below props to 0/Transparent because RNViewBase should handle them.
           * We don't want default values set only on the native side, as they break the layout.
           */
          .padding(0)
          .borderRadius(0)
          .backgroundColor(Color.Transparent)
          .caretColor(convertColorSegmentsToString(this.descriptor.props.selectionColor))
          .onFocus(() => this.onFocusEvent())
          .defaultFocus(false)
          .onBlur(() => this.onBlurEvent())/*
           * We need to handle padding manually as TextArea does not handle padding correctly
           * - it does not respect percentage padding and the scroll area size is to big */
          .position({ x: this.descriptorWrapper?.position.x, y: this.descriptorWrapper?.position.y })
          .width(this.descriptorWrapper?.contentWidth)
          .height(this.descriptorWrapper?.contentHeight)
          .responseRegion({
            x: -(this.descriptorWrapper?.position.x ?? 0),
            y: -(this.descriptorWrapper?.position.y ?? 0),
            width: this.descriptorWrapper?.width,
            height: this.descriptorWrapper?.height
          })
          .id(`TextInput@${this.tag}`)
          .onAppear(() => this.maybeFocusOnAppear())
          .focusable(this.descriptorWrapper?.focusable ?? true)
          .onPaste(() => this.onPaste())
          .onTextSelectionChange((start: number, end: number) => this.textSelectionChange(start, end))
          .type(getTextAreaType(this.descriptor.props.keyboardType))
          .onEditChange((isEditing: boolean) => {
            this.isEditing = isEditing;
          })
          .selectionMenuHidden(this.descriptorWrapper?.contextMenuHidden)
          .inputFilter('.*')
          .hitTestBehavior(this.descriptorWrapper?.pointerEvents === "none" ? HitTestMode.None :
          HitTestMode.Default)
      } else {
        TextInput({
          controller: this.inputController,
          placeholder: this.descriptor.props.placeholder,
          text: this.value
        })
          .fontColor(this.descriptorWrapper?.fontColor)
          .fontFamily(this.descriptorWrapper?.fontFamily)
          .fontSize(this.descriptorWrapper?.fontSize)
          .fontWeight(this.descriptorWrapper?.fontWeight)
          .fontStyle(stringToFontStyle(this.descriptorWrapper?.fontStyle))
          .onChange((event) => this.handleChangeEvent(event))
          .enabled(this.descriptor.props.editable ?? true)
          .caretStyle(getCaretStyle(this.descriptor.props.caretHidden))
          .type(getInputType(this.descriptor.props.keyboardType, this.descriptor.props.secureTextEntry))
          .maxLength(this.descriptor.rawProps.maxLength)
          .selectedBackgroundColor(convertColorSegmentsToString(this.descriptor.props.selectionColor))
          .caretColor(convertColorSegmentsToString(this.descriptor.props.selectionColor))
          .showPasswordIcon(false)
          .placeholderColor(convertColorSegmentsToString(this.descriptor.props.placeholderTextColor))
          .placeholderFont({
            size: this.descriptorWrapper?.fontSize,
            family: this.descriptorWrapper?.fontFamily ?? "HarmonyOS Sans",
            weight: this.descriptorWrapper?.fontWeight ?? FontWeight.Normal,
            style: stringToFontStyle(this.descriptorWrapper?.fontStyle)
          })
          .enterKeyType(stringToEnterKeyType(this.descriptor.props.returnKeyType))
          .textAlign(stringToAlignment(this.descriptor.props.textAlign))/*
           * We set the below props to 0/Transparent because RNViewBase should handle them.
           * We don't want default values set only on the native side, as they break the layout.
           */
          .padding(0)
          .borderRadius(0)
          .backgroundColor(Color.Transparent)
          .onSubmit(() => this.onSubmitEditing())
          .onFocus(() => this.onFocusEvent())
          .defaultFocus(false)
          .onBlur(() => this.onBlurEvent())/*
           * We need to handle padding manually as TextInput does not handle padding correctly
           * - the text is always centered. */
          .position({ x: this.descriptorWrapper?.position.x, y: this.descriptorWrapper?.position.y })
          .width(this.descriptorWrapper?.contentWidth)
          .height(this.descriptorWrapper?.contentHeight)
          .responseRegion({
            x: -(this.descriptorWrapper?.position.x ?? 0),
            y: -(this.descriptorWrapper?.position.y ?? 0),
            width: this.descriptorWrapper?.width,
            height: this.descriptorWrapper?.height
          })
          .id(`TextInput@${this.tag}`)
          .onAppear(() => this.maybeFocusOnAppear())
          .focusable(this.descriptorWrapper?.focusable ?? true)
          .cancelButton(this.getCancelButton())
          .onPaste(() => this.onPaste())
          .onTextSelectionChange((start: number, end: number) => this.textSelectionChange(start, end))
          .onEditChange((isEditing: boolean) => {
            this.isEditing = isEditing;
          })
          .selectionMenuHidden(this.descriptorWrapper?.contextMenuHidden)
          .inputFilter('.*')
          .hitTestBehavior(this.descriptorWrapper?.pointerEvents === "none" ? HitTestMode.None :
          HitTestMode.Default)
      }
    }
  }
}