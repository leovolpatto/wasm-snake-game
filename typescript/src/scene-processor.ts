import { Wasm } from "wasm.memory";

export class SceneProcessor {
    private ctx: CanvasRenderingContext2D;

    public constructor(
        private canvas: HTMLCanvasElement,
        private wasm: Wasm
    ) {
        this.ctx = canvas.getContext('2d', {
            willReadFrequently: true
        })!;
    }

    public processFrame(deltaTimeSecs: number): void {
        const bufferInfo = this.wasm.getSceneBufferInfo();

        const wasmBuffer = new Uint8ClampedArray(
            this.wasm.wasmMemory!.u8.buffer,
            bufferInfo.bufferAddr,
            bufferInfo.bufferSize
        );
        
        this.wasm.wasmModule!._processSceneBuffer(deltaTimeSecs);
        
        const processedImageData = new ImageData(
            wasmBuffer, 
            bufferInfo.width, 
            bufferInfo.height
        );
        
        this.ctx.putImageData(processedImageData, 0, 0);
    }
}