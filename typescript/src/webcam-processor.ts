import { Wasm } from "wasm.memory";

export class WebcamProcessor {
    private ctx: CanvasRenderingContext2D;

    public constructor(
        private canvas: HTMLCanvasElement,
        private wasm: Wasm
    ) {
        this.ctx = this.canvas.getContext('2d', {
            willReadFrequently: true
        })!;
    }

    public processWebcamFrame(videoElement: HTMLVideoElement, deltaTimeSecs: number): void {
        // 1. Primeiro pegamos as informações do buffer
        const bufferInfo = this.wasm.getWebcamBufferInfo();
        
        // 2. Desenhamos o frame da webcam no canvas
        this.ctx.drawImage(videoElement, 0, 0, bufferInfo.width, bufferInfo.height);
        
        // 3. Pegamos os pixels do canvas
        const imageData = this.ctx.getImageData(0, 0, bufferInfo.width, bufferInfo.height);
        
        // 4. Criamos uma view do buffer WASM usando o endereço do buffer
        const wasmBuffer = new Uint8ClampedArray(
            this.wasm.wasmMemory!.u8.buffer,
            bufferInfo.bufferAddr,
            bufferInfo.bufferSize
        );

        // 5. Copiamos os pixels do canvas para o buffer WASM
        wasmBuffer.set(imageData.data);
        
        // 6. Processamos o buffer no WASM
        this.wasm.wasmModule!._processWebcamBuffer(deltaTimeSecs);

        // 7. Criamos um novo ImageData com o buffer processado
        const processedImageData = new ImageData(
            wasmBuffer, 
            bufferInfo.width, 
            bufferInfo.height
        );
        // 8. Renderizamos o resultado processado no canvas
        this.ctx.putImageData(processedImageData, 0, 0);
    }
}