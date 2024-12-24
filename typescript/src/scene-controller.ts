import { Wasm } from "wasm.memory";

export class SceneController {
    private lastTime: number = 0;
    private ctx: CanvasRenderingContext2D
    public constructor(
        private wasmModule: Wasm,
        private width: number,
        private height: number
        
    ) {
        console.log('canvas is: ', width + ' x ' + height) ;
        this.wasmModule.wasmModule!._initScene(width, height);
    }

    public start() {
        let lastTime = 0;
        console.log("Starting animation loop");
        
        const animate = (currentTime: number) => {
            const deltaTime = (currentTime - lastTime) / 1000;
            lastTime = currentTime;
                
            this.wasmModule.wasmModule!._updateScene(deltaTime);
    
            const bufferPtr = this.wasmModule.wasmModule!._getSceneBuffer();
    
            const buffer = new Uint8ClampedArray(
                this.wasmModule.wasmMemory!.u8.buffer,
                bufferPtr,
                this.ctx.canvas.width * this.ctx.canvas.height * 4
            );
    
            const imageData = new ImageData(
                buffer,
                this.ctx.canvas.width,
                this.ctx.canvas.height
            );
            this.ctx.putImageData(imageData, 0, 0);
            
            requestAnimationFrame(animate);
        };
    
        requestAnimationFrame(animate);        
    }
}