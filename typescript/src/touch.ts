import { IInputObserver } from "input-observer.interface";

export class TouchInput {
    private touchStartX: number = 0;
    private touchStartY: number = 0;
    private touchEndX: number = 0;
    private touchEndY: number = 0;
    private observer: IInputObserver;

    public constructor(observer: IInputObserver) {
        this.observer = observer;
    }

    public start(): void {
        this.listenEvents();
    }

    private listenEvents(): void {
        
        document.ontouchstart = (evt: any) => this.handleTouchStart(evt);

        document.ontouchend = (evt: any) => this.handleTouchEnd(evt);

        document.addEventListener('touchmove', function (event) {
            event.preventDefault();
        }, { passive: false });
    }


    private handleTouchStart(event: any): void {
        this.touchStartX = event.touches[0].clientX;
        this.touchStartY = event.touches[0].clientY;
    }

    private handleTouchEnd(event: any): void {
        this.touchEndX = event.changedTouches[0].clientX;
        this.touchEndY = event.changedTouches[0].clientY;
        this.handleSwipe();
    }

    private handleSwipe(): void {
        const deltaX = this.touchEndX - this.touchStartX;
        const deltaY = this.touchEndY - this.touchStartY;

        if (Math.abs(deltaX) > Math.abs(deltaY)) {
            // Movimento horizontal
            if (deltaX > 0) {
                this.observer.right();
            } else {
                this.observer.left();
            }

            return;
        }

        // Movimento vertical
        if (deltaY > 0) {
            this.observer.down();
        } else {
            this.observer.up();
        }
    }
}