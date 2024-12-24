import { Canvas, Container, Video } from "./elements";

export class Screen{
    private canvasDbg!: Canvas;
    private camView!: Video;
    private container!: Container;

    public constructor(){
        this.container = new Container();
    }

    public getContainer(): Container{
        return this.container;
    }

    public async showDebugCamView(stream: MediaStream): Promise<void>{
        this.canvasDbg = Canvas.createInContainer({
            className: 'canva1'
        }, this.container);

        this.camView = Video.createInContainer({
            className: 'cam-view'
        }, this.container);

        this.camView.onPlay(ev => {
            this.renderFrame(ev);
        });

        this.camView.element.srcObject = stream;
        await this.camView.startVideo();
    }
    
    private renderFrame(ev: Event): void{
        const ctx = this.canvasDbg.element.getContext('2d')!;

        ctx.drawImage(this.camView.element, 0, 0, 300, 200);
        
        //const frameData = ctx.getImageData(0, 0, this.canvasDbg.element.width, this.canvasDbg.element.height);
        //processFrameWithWasm(frameData);
        requestAnimationFrame((x) => { 
            this.renderFrame(ev);
        });
    }    
}