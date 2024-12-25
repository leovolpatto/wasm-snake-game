import { IInputObserver } from "input-observer.interface";
import { KeyboardInput } from "keyboard";
import { SceneProcessor } from "scene-processor";
import { TouchInput } from "touch";
import { VideoEffectsProcessor } from "video-effects-processor";
import { InputKeys, Wasm } from "wasm.memory";
import { Webcam } from "webcam";
import { WebcamProcessor } from "webcam-processor";

export class Game implements IInputObserver {
  private wasm!: Wasm;
  private canvasRawVideo: HTMLCanvasElement;
  private canvasEffectsVideo: HTMLCanvasElement;
  private canvasScene: HTMLCanvasElement;

  private webCamProcessor!: WebcamProcessor;
  private videoEffectsProcessor!: VideoEffectsProcessor;
  private sceneProcessor!: SceneProcessor;

  public constructor() {
    this.canvasRawVideo = document.getElementById(
      "cam-raw-view"
    )! as HTMLCanvasElement;

    this.canvasEffectsVideo = document.getElementById(
      "cam-processed-view"
    )! as HTMLCanvasElement;

    this.canvasScene = document.getElementById(
      "game-canvas"
    )! as HTMLCanvasElement;

    this.wasm = new Wasm();
  }

  public right(): void {
    this.wasm.wasmModule?._processKeyPressed(InputKeys.Right);
  }

  public left(): void {
    this.wasm.wasmModule?._processKeyPressed(InputKeys.Left);
  }

  public up(): void {
    this.wasm.wasmModule?._processKeyPressed(InputKeys.Up);
  }

  public down(): void {
    this.wasm.wasmModule?._processKeyPressed(InputKeys.Down);
  }

  public pause(): void {
    this.wasm.wasmModule?._processKeyPressed(InputKeys.Space);
  }

  private initInputs(): void {
    const keyboard: KeyboardInput = new KeyboardInput(this);
    const touch: TouchInput = new TouchInput(this);

    keyboard.start();
    touch.start();
  }

  private async initCamera(): Promise<HTMLVideoElement> {
    console.log('Requesting camera w: ' + this.canvasRawVideo.width + " h: " + this.canvasRawVideo.height);

    const stream = await Webcam.getMediaStream(this.canvasRawVideo.width);

    const video = document.createElement("video");
    video.hidden = true;
    video.srcObject = stream;
    video.width = this.canvasRawVideo.width;
    video.height = this.canvasRawVideo.height;
    await video.play();

    return video;
  }

  private initWasmBuffers(): void{
    this.wasm.wasmModule?._initVideoBuffers(
      this.canvasRawVideo.width,
      this.canvasRawVideo.height
    );

    this.wasm.wasmModule?._initSceneBuffer(
      900, //this.canvasScene.width,
      500 //this.canvasScene.height
    );
  }

  private initVideoProcessors(): void{
    this.webCamProcessor = new WebcamProcessor(this.canvasRawVideo, this.wasm);
    this.videoEffectsProcessor = new VideoEffectsProcessor(
      this.canvasEffectsVideo,
      this.wasm
    );
    this.sceneProcessor = new SceneProcessor(this.canvasScene, this.wasm);
  }

  private async init(): Promise<void> {
    this.initInputs();
    const video: HTMLVideoElement = await this.initCamera();
    this.initWasmBuffers();
    this.initVideoProcessors();

    this.startProcessingVideo(video);
  }

  public startProcessingVideo(videoElement: HTMLVideoElement): void {
    let lastTime: number = 0;

    const animate = (currentTime: number) => {
      const deltaTimeSecs = lastTime ? (currentTime - lastTime) / 1000 : 0;
      lastTime = currentTime;

      this.webCamProcessor.processWebcamFrame(videoElement, deltaTimeSecs);
      this.sceneProcessor.processFrame(deltaTimeSecs);
      this.videoEffectsProcessor.processFrame(deltaTimeSecs);

      requestAnimationFrame(animate);
    };

    requestAnimationFrame(animate);
  }

  public async play(): Promise<void> {
    try {
      await this.wasm.initWasm();
      await this.init();
    } catch (error) {
      console.error("Error initializing game:", error);
    }
  }
}
