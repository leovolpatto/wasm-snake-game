import { Canvas, Container, Panel, Video } from "elements";
import { Food } from "./food";
import { Snake } from "./snake";
import { Screen } from "screen";
import { Wasm } from "wasm.memory";
import { SceneController } from "scene-controller";

export class Scenario {
  private scenarioDiv!: Panel;
  private screenContainer: Container;
  private scenarioContainer!: Container;
  private snakeCanvas!: Canvas;
  private foodCanvas!: Canvas;
  private wasm!: Wasm;
  private readonly MAX_WIDTH = 900;
  private readonly MAX_HEIGHT = 500;

  private bufferPtr: number = 0;
  private imageBuffer!: Uint8Array<ArrayBufferLike>;

  private sceneController!: SceneController;
  private gameCanvas!: Canvas;

  public constructor(private screen: Screen) {
    this.screenContainer = screen.getContainer();

    this.gameCanvas = Canvas.createInContainer(
      { className: "game-canvas" },
      this.screenContainer
    );

    this.buildScenarioCanvas();
    this.buildScenarioContainer();
  }

  public setWasm(wasm: Wasm): void {
    this.wasm = wasm;

    console.info("Getting ready to read WASM memory...");

    const memoryNeeded = this.MAX_WIDTH * this.MAX_HEIGHT * 4; //RGBA
    this.wasm.wasmModule?._allocateMemoryForWebcam(this.MAX_WIDTH, this.MAX_HEIGHT);

    this.bufferPtr = wasm.wasmModule?._getBufferPointer()!;
    this.imageBuffer = this.wasm.wasmModule?.HEAPU8.subarray(
      this.bufferPtr!,
      this.bufferPtr! + memoryNeeded
    )!;

    console.info("WASM memory buffer is ", this.bufferPtr);
    console.info("WASM image memory lenght is ", this.imageBuffer.length);

    this.sceneController = new SceneController(
      wasm,
      this.MAX_WIDTH,
      this.MAX_HEIGHT,
      this.gameCanvas.element.getContext("2d")!
    );

    this.sceneController.start();
  }

  public buildScenarioContainer(): void {
    this.scenarioContainer = new Container(this.scenarioDiv.element);
  }

  private buildScenarioCanvas(): void {
    this.scenarioDiv = Panel.createInContainer(
      { className: "scenario-div" },
      this.screenContainer
    );
  }

  public async compose(snake: Snake, food: Food): Promise<void> {
    this.snakeCanvas = Canvas.createInContainer(
      { className: "snake" },
      this.scenarioContainer
    );
    this.foodCanvas = Canvas.createInContainer(
      { className: "food" },
      this.scenarioContainer
    );
  }

  private processFrameWithWasm(
    imageData: ImageData,
    ctx: CanvasRenderingContext2D
  ) {
    this.imageBuffer?.set(imageData.data);
    this.wasm.wasmModule?._processBuffer(imageData.width, imageData.height);

    imageData.data.set(
      this.imageBuffer.subarray(0, imageData.width * imageData.height * 4)
    );
    ctx.putImageData(imageData, 0, 0);
  }

  public async renderFrame(ev: Event, videoElement: Video): Promise<void> {
    await this.renderSnake(videoElement);
    await this.renderFood(videoElement);
    requestAnimationFrame((x) => {
      this.renderFrame(ev, videoElement);
    });
  }

  public async renderSnake(videoElement: Video): Promise<void> {
    const ctx: CanvasRenderingContext2D =
      this.snakeCanvas.element.getContext("2d")!;
    ctx.drawImage(
      videoElement.element,
      0,
      0,
      this.snakeCanvas.element.width,
      this.snakeCanvas.element.height
    );
    const frameData: ImageData = ctx.getImageData(
      10,
      0,
      this.snakeCanvas.element.width,
      this.snakeCanvas.element.height
    );
    this.processFrameWithWasm(frameData, ctx);
  }

  public async renderFood(videoElement: Video): Promise<void> {
    const ctx = this.foodCanvas.element.getContext("2d")!;
    ctx.drawImage(
      videoElement.element,
      0,
      0,
      this.foodCanvas.element.width,
      this.foodCanvas.element.height
    );
    const frameData = ctx.getImageData(
      10,
      0,
      this.foodCanvas.element.width,
      this.foodCanvas.element.height
    );
    //processFrameWithWasm(frameData);
  }
}
