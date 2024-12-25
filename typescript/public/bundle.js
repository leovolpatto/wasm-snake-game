"use strict";
(() => {
  // src/keyboard.ts
  var KeyboardInput = class {
    constructor(observer) {
      this.observer = observer;
    }
    start() {
      this.listenEvents();
    }
    listenEvents() {
      document.onkeydown = (evt) => {
        switch (evt.code) {
          case "ArrowUp":
            this.observer.up();
            break;
          case "ArrowDown":
            this.observer.down();
            break;
          case "ArrowLeft":
            this.observer.left();
            break;
          case "ArrowRight":
            this.observer.right();
            break;
          case "Space":
            this.observer.pause();
            break;
        }
      };
    }
  };

  // src/scene-processor.ts
  var SceneProcessor = class {
    constructor(canvas, wasm) {
      this.canvas = canvas;
      this.wasm = wasm;
      this.ctx = canvas.getContext("2d", {
        willReadFrequently: true
      });
    }
    processFrame(deltaTimeSecs) {
      const bufferInfo = this.wasm.getSceneBufferInfo();
      const wasmBuffer = new Uint8ClampedArray(
        this.wasm.wasmMemory.u8.buffer,
        bufferInfo.bufferAddr,
        bufferInfo.bufferSize
      );
      this.wasm.wasmModule._processSceneBuffer(deltaTimeSecs);
      const processedImageData = new ImageData(
        wasmBuffer,
        bufferInfo.width,
        bufferInfo.height
      );
      this.ctx.putImageData(processedImageData, 0, 0);
    }
  };

  // src/touch.ts
  var TouchInput = class {
    constructor(observer) {
      this.touchStartX = 0;
      this.touchStartY = 0;
      this.touchEndX = 0;
      this.touchEndY = 0;
      this.observer = observer;
    }
    start() {
      this.listenEvents();
    }
    listenEvents() {
      document.ontouchstart = (evt) => this.handleTouchStart(evt);
      document.ontouchend = (evt) => this.handleTouchEnd(evt);
      document.addEventListener("touchmove", function(event) {
        event.preventDefault();
      }, { passive: false });
    }
    handleTouchStart(event) {
      this.touchStartX = event.touches[0].clientX;
      this.touchStartY = event.touches[0].clientY;
    }
    handleTouchEnd(event) {
      this.touchEndX = event.changedTouches[0].clientX;
      this.touchEndY = event.changedTouches[0].clientY;
      this.handleSwipe();
    }
    handleSwipe() {
      const deltaX = this.touchEndX - this.touchStartX;
      const deltaY = this.touchEndY - this.touchStartY;
      if (Math.abs(deltaX) > Math.abs(deltaY)) {
        if (deltaX > 0) {
          this.observer.right();
        } else {
          this.observer.left();
        }
        return;
      }
      if (deltaY > 0) {
        this.observer.down();
      } else {
        this.observer.up();
      }
    }
  };

  // src/video-effects-processor.ts
  var VideoEffectsProcessor = class {
    constructor(canvas, wasm) {
      this.canvas = canvas;
      this.wasm = wasm;
      this.ctx = canvas.getContext("2d", {
        willReadFrequently: true
      });
    }
    processFrame(deltaTimeSecs) {
      const bufferInfo = this.wasm.getVideoBufferInfo();
      const wasmBuffer = new Uint8ClampedArray(
        this.wasm.wasmMemory.u8.buffer,
        bufferInfo.bufferAddr,
        bufferInfo.bufferSize
      );
      this.wasm.wasmModule._processVideoBuffer(deltaTimeSecs);
      const processedImageData = new ImageData(
        wasmBuffer,
        bufferInfo.width,
        bufferInfo.height
      );
      this.ctx.putImageData(processedImageData, 0, 0);
    }
  };

  // src/wasm.memory.ts
  var Wasm = class {
    async initWasm() {
      return new Promise((resolve) => {
        Module.onRuntimeInitialized = () => {
          console.log("WASM runtime initialized");
          this.wasmModule = Module;
          this.wasmMemory = {
            u8: Module.HEAPU8,
            // Para dados de 8 bits sem sinal
            u32: Module.HEAPU32,
            // Para inteiros de 32 bits sem sinal
            f32: Module.HEAPF32
            // Para números de ponto flutuante
          };
          this.wasmModule?._init();
          this.wasmModule._initialize();
          console.log("WASM module initialized");
          resolve();
        };
      });
    }
    getBufferInfo(infoPtr) {
      const view = new DataView(this.wasmMemory.u8.buffer);
      return {
        bufferAddr: view.getUint32(infoPtr, true),
        width: view.getInt32(infoPtr + 4, true),
        height: view.getInt32(infoPtr + 8, true),
        bufferSize: view.getInt32(infoPtr + 12, true),
        initialized: Boolean(view.getUint8(infoPtr + 16))
      };
    }
    getSceneBufferInfo() {
      return this.getBufferInfo(this.wasmModule._getSceneBufferInfo());
    }
    getWebcamBufferInfo() {
      return this.getBufferInfo(this.wasmModule._getWebcamBufferInfo());
    }
    getVideoBufferInfo() {
      return this.getBufferInfo(this.wasmModule._getVideoBufferInfo());
    }
    copyToWasmMemory(data, ptr) {
      this.wasmMemory.u8.set(data, ptr);
    }
  };

  // src/webcam.ts
  var Webcam = class _Webcam {
    constructor() {
    }
    static async getMediaStream(width) {
      if (_Webcam.stream) {
        return _Webcam.stream;
      }
      _Webcam.stream = await navigator.mediaDevices.getUserMedia({
        video: {
          frameRate: 80
        },
        audio: false
      });
      return _Webcam.stream;
    }
  };

  // src/webcam-processor.ts
  var WebcamProcessor = class {
    constructor(canvas, wasm) {
      this.canvas = canvas;
      this.wasm = wasm;
      this.ctx = this.canvas.getContext("2d", {
        willReadFrequently: true
      });
      this.bufferInfo = this.wasm.getWebcamBufferInfo();
      this.memoryView = new Uint8ClampedArray(
        this.wasm.wasmMemory.u8.buffer,
        this.bufferInfo.bufferAddr,
        this.bufferInfo.bufferSize
      );
    }
    drawWebcamFrameToCanvas(videoElement) {
      this.ctx.drawImage(videoElement, 0, 0, this.bufferInfo.width, this.bufferInfo.height);
    }
    getCanvasCurrentImageData() {
      const imageData = this.ctx.getImageData(0, 0, this.bufferInfo.width, this.bufferInfo.height);
      return imageData;
    }
    updateWasmMemoryBufferWith(imageData) {
      this.memoryView.set(imageData.data);
    }
    processDataInWasm(deltaTimeSecs) {
      this.wasm.wasmModule._processWebcamBuffer(deltaTimeSecs);
    }
    getProcessedDataFromWasm() {
      const processedImageData = new ImageData(
        this.memoryView,
        this.bufferInfo.width,
        this.bufferInfo.height
      );
      return processedImageData;
    }
    drawToCanvas(imageData) {
      this.ctx.putImageData(imageData, 0, 0);
    }
    processWebcamFrame(videoElement, deltaTimeSecs) {
      this.drawWebcamFrameToCanvas(videoElement);
      const imageData = this.getCanvasCurrentImageData();
      this.updateWasmMemoryBufferWith(imageData);
      this.processDataInWasm(deltaTimeSecs);
      const data = this.getProcessedDataFromWasm();
      this.drawToCanvas(data);
    }
  };

  // src/game.ts
  var Game = class {
    constructor() {
      this.canvasRawVideo = document.getElementById(
        "cam-raw-view"
      );
      this.canvasEffectsVideo = document.getElementById(
        "cam-processed-view"
      );
      this.canvasScene = document.getElementById(
        "game-canvas"
      );
      this.wasm = new Wasm();
    }
    right() {
      this.wasm.wasmModule?._processKeyPressed(1 /* Right */);
    }
    left() {
      this.wasm.wasmModule?._processKeyPressed(3 /* Left */);
    }
    up() {
      this.wasm.wasmModule?._processKeyPressed(0 /* Up */);
    }
    down() {
      this.wasm.wasmModule?._processKeyPressed(2 /* Down */);
    }
    pause() {
      this.wasm.wasmModule?._processKeyPressed(4 /* Space */);
    }
    initInputs() {
      const keyboard = new KeyboardInput(this);
      const touch = new TouchInput(this);
      keyboard.start();
      touch.start();
    }
    async initCamera() {
      console.log("Requesting camera w: " + this.canvasRawVideo.width + " h: " + this.canvasRawVideo.height);
      const stream = await Webcam.getMediaStream(this.canvasRawVideo.width);
      const video = document.createElement("video");
      video.hidden = true;
      video.srcObject = stream;
      video.width = this.canvasRawVideo.width;
      video.height = this.canvasRawVideo.height;
      await video.play();
      return video;
    }
    initWasmBuffers() {
      this.wasm.wasmModule?._initVideoBuffers(
        this.canvasRawVideo.width,
        this.canvasRawVideo.height
      );
      this.wasm.wasmModule?._initSceneBuffer(
        900,
        //this.canvasScene.width,
        500
        //this.canvasScene.height
      );
    }
    initVideoProcessors() {
      this.webCamProcessor = new WebcamProcessor(this.canvasRawVideo, this.wasm);
      this.videoEffectsProcessor = new VideoEffectsProcessor(
        this.canvasEffectsVideo,
        this.wasm
      );
      this.sceneProcessor = new SceneProcessor(this.canvasScene, this.wasm);
    }
    async init() {
      this.initInputs();
      const video = await this.initCamera();
      this.initWasmBuffers();
      this.initVideoProcessors();
      this.startProcessingVideo(video);
    }
    startProcessingVideo(videoElement) {
      let lastTime = 0;
      const animate = (currentTime) => {
        const deltaTimeSecs = lastTime ? (currentTime - lastTime) / 1e3 : 0;
        lastTime = currentTime;
        this.webCamProcessor.processWebcamFrame(videoElement, deltaTimeSecs);
        this.sceneProcessor.processFrame(deltaTimeSecs);
        this.videoEffectsProcessor.processFrame(deltaTimeSecs);
        requestAnimationFrame(animate);
      };
      requestAnimationFrame(animate);
    }
    async play() {
      try {
        await this.wasm.initWasm();
        await this.init();
      } catch (error) {
        console.error("Error initializing game:", error);
      }
    }
  };

  // src/main.ts
  (async () => {
    const game = new Game();
    game.play();
  })();
})();
