import { Container } from "./container";
import { Element, IElementAttributes } from "./element";

export class Video extends Element<HTMLVideoElement>{
    private constructor(private initialAttributes: IElementAttributes, public element: HTMLVideoElement){
        super(initialAttributes, element);
    }

    public static createInContainer(initialAttributes: IElementAttributes, container: Container): Video{
        const video: Video = new Video(initialAttributes, container.document.createElement("video"));
        video.element.className = initialAttributes.className ?? '';
        
        container.parentElement.appendChild(video.element);
        
        container.parentElement.appendChild(video.element);

        return video;
    }

    public onLoadMetadata(loaded: (ev: Event) => void): void{
        this.element.addEventListener('loadedmetadata', (ev) => {
            loaded(ev);
        });
    }

    public onPlay(play: (ev: Event) => void): void{
        this.element.addEventListener('play', (ev) => {
            play(ev);
        });
    }

    public async startVideo(): Promise<void>{
        await this.element.play();
    }
}