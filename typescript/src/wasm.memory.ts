export interface BufferInfo {
    bufferAddr: number;
    bufferSize: number;
    width: number;
    height: number;
    initialized: boolean;
}

export enum InputKeys
{
    Up = 0,
    Right = 1,
    Down = 2,
    Left = 3,
    Space = 4
}

export class Wasm{
    public wasmModule?: SnakeModule;
    public wasmMemory?: {
        u8: Uint8Array;
        u32: Uint32Array;
        f32: Float32Array;
    };

    public async initWasm(): Promise<void> {
        return new Promise((resolve) => {
            Module.onRuntimeInitialized = () => {
                console.log("WASM runtime initialized");
                this.wasmModule = Module;
                // Inicializando views da memória
                this.wasmMemory = {
                    u8: Module.HEAPU8,    // Para dados de 8 bits sem sinal
                    u32: Module.HEAPU32,  // Para inteiros de 32 bits sem sinal
                    f32: Module.HEAPF32   // Para números de ponto flutuante
                };

                this.wasmModule?._init();
                this.wasmModule._initialize();

                console.log('WASM module initialized');
                resolve();
            };
        });
    }
    
    private getBufferInfo(infoPtr: number): BufferInfo {
        const view = new DataView(this.wasmMemory!.u8.buffer);
        
        return {
            bufferAddr: view.getUint32(infoPtr, true),
            width: view.getInt32(infoPtr + 4, true),
            height: view.getInt32(infoPtr + 8, true),
            bufferSize: view.getInt32(infoPtr + 12, true),
            initialized: Boolean(view.getUint8(infoPtr + 16))
        };
    }

    public getSceneBufferInfo(): BufferInfo {
        return this.getBufferInfo(this.wasmModule!._getSceneBufferInfo());
    }    

    public getWebcamBufferInfo(): BufferInfo {
        return this.getBufferInfo(this.wasmModule!._getWebcamBufferInfo());
    } 

    public getVideoBufferInfo(): BufferInfo {
        return this.getBufferInfo(this.wasmModule!._getVideoBufferInfo());
    }  

    private copyToWasmMemory(data: Uint8Array, ptr: number): void {
        this.wasmMemory!.u8.set(data, ptr);
    }

    // Método auxiliar para ler dados da memória WASM
    private readFromWasmMemory(ptr: number, size: number): Uint8Array {
        return this.wasmMemory!.u8.slice(ptr, ptr + size);
    }
}
