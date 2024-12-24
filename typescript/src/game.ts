import { SceneProcessor } from "scene-processor";
import { VideoEffectsProcessor } from "video-effects-processor";
import { InputKeys, Wasm } from "wasm.memory";
import { WebcamProcessor } from "webcam-processor";

export class Game {
  private wasm!: Wasm;
  private canvasRawVideo: HTMLCanvasElement;
  private canvasEffectsVideo: HTMLCanvasElement;
  private canvasScene: HTMLCanvasElement;
  //private scene!: SceneController;

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

  private async init(): Promise<void> {
    document.onkeydown = (evt: KeyboardEvent) => {
      switch (evt.code) {
        case "ArrowUp":
          this.wasm.wasmModule?._processKeyPressed(InputKeys.Up);
          break;
        case "ArrowDown":
          this.wasm.wasmModule?._processKeyPressed(InputKeys.Down);
          break;
        case "ArrowLeft":
          this.wasm.wasmModule?._processKeyPressed(InputKeys.Left);
          break;
        case "ArrowRight":
          this.wasm.wasmModule?._processKeyPressed(InputKeys.Right);
          break;
        case "Space":
          this.wasm.wasmModule?._processKeyPressed(InputKeys.Space);
          break;
      }
    };

    this.wasm.wasmModule?._initVideoBuffers(
      this.canvasRawVideo.width,
      this.canvasRawVideo.height
    );

    this.wasm.wasmModule?._initSceneBuffer(
      900, //this.canvasScene.width,
      500 //this.canvasScene.height
    );

    const stream = await navigator.mediaDevices.getUserMedia({
      video: {
        width: {
          exact: this.canvasRawVideo.width,
        },
        height: {
          exact: this.canvasRawVideo.height,
        },
        frameRate: 80,
      },
    });

    const video = document.createElement("video");
    video.hidden = true;
    video.srcObject = stream;
    video.width = this.canvasRawVideo.width;
    video.height = this.canvasRawVideo.height;
    await video.play();

    // Iniciar processamento
    this.webCamProcessor = new WebcamProcessor(this.canvasRawVideo, this.wasm);
    this.videoEffectsProcessor = new VideoEffectsProcessor(
      this.canvasEffectsVideo,
      this.wasm
    );
    this.sceneProcessor = new SceneProcessor(this.canvasScene, this.wasm);

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

  private async setScene(): Promise<void> {
    //this.scene = new SceneController(this.wasm, 800,500)
  }

  public async play(): Promise<void> {
    try {
      await this.wasm.initWasm();
      await this.init();
      await this.setScene();
    } catch (error) {
      console.error("Error initializing game:", error);
    }
  }
}
