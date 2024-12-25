import { BufferInfo, Wasm } from "wasm.memory";

export class WebcamProcessor {
    private ctx: CanvasRenderingContext2D;
    private bufferInfo: BufferInfo;
    private memoryView: Uint8ClampedArray;

    public constructor(
        private canvas: HTMLCanvasElement,
        private wasm: Wasm
    ) {
        this.ctx = this.canvas.getContext('2d', {
            willReadFrequently: true
        })!;

        this.bufferInfo = this.wasm.getWebcamBufferInfo();
        this.memoryView = new Uint8ClampedArray(
            this.wasm.wasmMemory!.u8.buffer,
            this.bufferInfo.bufferAddr,
            this.bufferInfo.bufferSize
        );   
    }

    private drawWebcamFrameToCanvas(videoElement: HTMLVideoElement): void{
        this.ctx.drawImage(videoElement, 0, 0, this.bufferInfo.width, this.bufferInfo.height);
    }

    private getCanvasCurrentImageData(): ImageData{
        const imageData:ImageData = this.ctx.getImageData(0, 0, this.bufferInfo.width, this.bufferInfo.height);
        return imageData;
    }

    private updateWasmMemoryBufferWith(imageData:ImageData): void{
        this.memoryView.set(imageData.data);
    }

    private processDataInWasm(deltaTimeSecs: number){
        this.wasm.wasmModule!._processWebcamBuffer(deltaTimeSecs);
    }

    private getProcessedDataFromWasm():ImageData{
        const processedImageData: ImageData = new ImageData(
            this.memoryView, 
            this.bufferInfo.width, 
            this.bufferInfo.height
        );

        return processedImageData;
    }

    private drawToCanvas(imageData: ImageData): void{
        this.ctx.putImageData(imageData, 0, 0);
    }

    public processWebcamFrame(videoElement: HTMLVideoElement, deltaTimeSecs: number): void {        
        this.drawWebcamFrameToCanvas(videoElement);

        const imageData:ImageData = this.getCanvasCurrentImageData();
        this.updateWasmMemoryBufferWith(imageData);
        
        this.processDataInWasm(deltaTimeSecs);

        const data: ImageData = this.getProcessedDataFromWasm();
        this.drawToCanvas(data);        
    }
}