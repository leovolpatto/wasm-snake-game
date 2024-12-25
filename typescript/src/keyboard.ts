import { IInputObserver } from "input-observer.interface";

export class KeyboardInput {
    private observer: IInputObserver;

    public constructor(observer: IInputObserver) {
        this.observer = observer;
    }

    public start(): void {
        this.listenEvents();
    }

    private listenEvents(): void {
        document.onkeydown = (evt: KeyboardEvent) => {
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
}