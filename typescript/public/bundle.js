"use strict";
(() => {
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
    // Método auxiliar para ler dados da memória WASM
    readFromWasmMemory(ptr, size) {
      return this.wasmMemory.u8.slice(ptr, ptr + size);
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
    }
    processWebcamFrame(videoElement, deltaTimeSecs) {
      const bufferInfo = this.wasm.getWebcamBufferInfo();
      this.ctx.drawImage(videoElement, 0, 0, bufferInfo.width, bufferInfo.height);
      const imageData = this.ctx.getImageData(0, 0, bufferInfo.width, bufferInfo.height);
      const wasmBuffer = new Uint8ClampedArray(
        this.wasm.wasmMemory.u8.buffer,
        bufferInfo.bufferAddr,
        bufferInfo.bufferSize
      );
      wasmBuffer.set(imageData.data);
      this.wasm.wasmModule._processWebcamBuffer(deltaTimeSecs);
      const processedImageData = new ImageData(
        wasmBuffer,
        bufferInfo.width,
        bufferInfo.height
      );
      this.ctx.putImageData(processedImageData, 0, 0);
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
    async init() {
      document.onkeydown = (evt) => {
        switch (evt.code) {
          case "ArrowUp":
            this.wasm.wasmModule?._processKeyPressed(0 /* Up */);
            break;
          case "ArrowDown":
            this.wasm.wasmModule?._processKeyPressed(2 /* Down */);
            break;
          case "ArrowLeft":
            this.wasm.wasmModule?._processKeyPressed(3 /* Left */);
            break;
          case "ArrowRight":
            this.wasm.wasmModule?._processKeyPressed(1 /* Right */);
            break;
          case "Space":
            this.wasm.wasmModule?._processKeyPressed(4 /* Space */);
            break;
        }
      };
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
      const stream = await navigator.mediaDevices.getUserMedia({
        video: {
          width: {
            exact: this.canvasRawVideo.width
          },
          height: {
            exact: this.canvasRawVideo.height
          },
          frameRate: 80
        }
      });
      const video = document.createElement("video");
      video.hidden = true;
      video.srcObject = stream;
      video.width = this.canvasRawVideo.width;
      video.height = this.canvasRawVideo.height;
      await video.play();
      this.webCamProcessor = new WebcamProcessor(this.canvasRawVideo, this.wasm);
      this.videoEffectsProcessor = new VideoEffectsProcessor(
        this.canvasEffectsVideo,
        this.wasm
      );
      this.sceneProcessor = new SceneProcessor(this.canvasScene, this.wasm);
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
    async setScene() {
    }
    async play() {
      try {
        await this.wasm.initWasm();
        await this.init();
        await this.setScene();
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
