/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import hilog from '@ohos.hilog';
import hiTrace from '@ohos.hiTraceMeter';
import { RNInstanceError, RNOHError } from "./RNOHError"

export interface RNOHLogger {
  info(...args: any[]): void

  warn(...args: any[]): void

  error(...args: any[]): void

  fatal(...args: any[]): void

  debug(...args: any[]): void

  clone(pathSegment: string | string[]): RNOHLogger

  startTracing(): () => void
}

export class Tracer {
  private activeTracesCount: number = 0

  public startTrace(name: string) {
    this.activeTracesCount++
    return () => {
      this.activeTracesCount--
    }
  }

  public getActiveTracesCount(): number {
    return this.activeTracesCount
  }
}

type Severity = "info" | "debug" | "fatal" | "error" | "warn"


export class StandardRNOHLogger implements RNOHLogger {
  /**
   * @deprecated: Throttling isn't supported anymore.
   */
  public static THROTTLE_IN_MS = 0

  private threadPrefix = "█____"
  private formattedPath: string = ""
  private severityValueByName: Record<Severity, number> = {
    debug: 0,
    info: 1,
    warn: 2,
    error: 3,
    fatal: 4,
  }

  constructor(
    private minSeverity: Severity = "debug",
    protected pathSegments: string[] = [],
    protected tracer: Tracer | undefined = undefined) {
    const formattedPath = this.formatPathSegments(pathSegments)
    if (formattedPath) {
      this.formattedPath = `${formattedPath} `
    }
    if (!this.tracer) {
      this.tracer = new Tracer()
    }
  }

  public setThreadPrefix(threadPrefix: string) {
    this.threadPrefix = threadPrefix
    return this
  }

  public setMinSeverity(minSeverity: Severity) {
    this.minSeverity = minSeverity
  }

  public info(...args: any[]): void {
    this.log("info", this.getCurrentOffset(), ...args)
  }

  public warn(...args: any[]): void {
    this.log("warn", this.getCurrentOffset(), ...args)
  }

  public error(...args: any[]): void {
    if (args[0] instanceof RNOHError) {
      this.log("error", this.getCurrentOffset(), this.stringifyRNOHError(args[0]))
    } else {
      this.log("error", this.getCurrentOffset(), ...args)
    }
  }

  private stringifyRNOHError(rnohError: RNOHError): string {
    if (rnohError instanceof RNInstanceError) {
      let rnInstanceNameString = "";
      if (rnohError.getRNInstanceName()) {
        rnInstanceNameString = `rnInstanceName="${rnohError.getRNInstanceName()}", `;
      }
      return `(${rnInstanceNameString}rnInstanceId=${rnohError.getRNInstanceId()}) ` +
      rnohError.getMessage() + "\n" + rnohError.getSuggestions().join("\n")
    } else {
      return rnohError.getMessage() + "\n" + rnohError.getSuggestions().join("\n");
    }
  }

  public fatal(...args: any[]): void {
    if (args[0] instanceof RNOHError) {
      hilog.fatal(this.getDomain(), this.getTag(), `${this.threadPrefix} %{public}s`, this.stringifyRNOHError(args[0]))
    } else {
      hilog.fatal(this.getDomain(), this.getTag(), `${this.threadPrefix} %{public}s`, ...args)
    }
  }

  public debug(...args: any[]): void {
    this.log("debug", this.getCurrentOffset(), ...args)
  }

  protected getDomain(): number {
    return 0xBEEF;
  }

  protected getTag(): string {
    return '#RNOH_ARK'
  }

  protected log(severity: Severity, offset: number, ...args: any[]): void {
    if (this.severityValueByName[severity] < this.severityValueByName[this.minSeverity]) {
      return;
    }
    if (args.length === 0) {
      args.push("")
    }
    const concatenatedArgs = args.map(arg => String(arg)).join(' ');
    hilog[severity](
      this.getDomain(),
      this.getTag(),
      `${this.threadPrefix} ${this.formattedPath} %{public}s`,
      concatenatedArgs,
    );
  }


  private getCurrentOffset(): number {
    if (this.minSeverity !== "debug") {
      return 0
    }
    return this.tracer.getActiveTracesCount() * 2
  }

  public clone(pathSegment: string | string[]): RNOHLogger {
    const newPathSegments = Array.isArray(pathSegment) ? pathSegment : [pathSegment]
    const newLogger = new StandardRNOHLogger(this.minSeverity, [...this.pathSegments, ...newPathSegments], this.tracer)
    newLogger.setThreadPrefix(this.threadPrefix)
    return newLogger
  }

  private formatPathSegments(pathSegments: string[]): string | null {
    if (pathSegments.length === 0) {
      return null
    }
    return pathSegments.join("::")
  }

  startTracing() {
    const startTime = Date.now()
    const currentOffset = this.getCurrentOffset()
    this.log("debug", currentOffset, "START")
    const stopTrace = this.tracer.startTrace(this.pathSegments.join('::'))
    return () => {
      stopTrace()
      const stopTime = Date.now()
      const durationInMs = stopTime - startTime
      this.log(durationInMs > 100 ? "warn" : "debug", currentOffset, `STOP (${durationInMs} ms)`)
    }
  }
}

