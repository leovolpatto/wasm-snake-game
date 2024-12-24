declare global {
    interface SnakeModule extends EmscriptenModule {
        _init(): void;
        _applySnakeHeadEffect(): void;
        _malloc(size: number): number;
        _free(ptr: number): void;
        _initializeBuffer(): void;
        _getBufferPointer(): number;
        _processBuffer(width: number, height: number): void;
        _allocateMemoryForWebcam(width: number, height: number): void;
        _freeBuffer(): void;
        _getSceneBuffer():number;

        _initialize():number;
        
        _initVideoBuffers(width: number, height: number): void;
        _initSceneBuffer(width: number, height: number): void;
        _getSceneBuffer(): number;
        _getWebcamBuffer(): number;
        _getVideoBuffer(): number;
        _processSceneBuffer(deltaTime: number): void;
        _processWebcamBuffer(deltaTime: number): void;
        _processVideoBuffer(deltaTime: number): void;
        _cleanupBuffers(): void;
        _processKeyPressed(key: number):void;
        
        /** Ponteiro para BufferInfo */
        _getSceneBufferInfo():number;

        /** Ponteiro para BufferInfo */
        _getWebcamBufferInfo():number;

        /** Ponteiro para BufferInfo */
        _getVideoBufferInfo():number;
        
        ccall(ident: string, returnType: string | null, argTypes: string[], args: any[]): any;
        cwrap(ident: string, returnType: string | null, argTypes: string[]): (...args: any[]) => any;

        HEAP8: Int8Array;
        HEAPU8: Uint8Array;
        HEAP16: Int16Array;
        HEAPU16: Uint16Array;
        HEAP32: Int32Array;
        HEAPU32: Uint32Array;
        HEAPF32: Float32Array;
        HEAPF64: Float64Array;
        
        buffer: ArrayBuffer;
    }

    interface EmscriptenModule {
        onRuntimeInitialized: () => void;
    }

    const Module: SnakeModule;
}

export {};