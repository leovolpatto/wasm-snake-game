import { Container } from "./container";
import { Element, IElementAttributes } from "./element";

export class Canvas extends Element<HTMLCanvasElement>{
    private constructor(private initialAttributes: IElementAttributes, public element: HTMLCanvasElement){
        super(initialAttributes, element);
    }

    public static createInContainer(initialAttributes: IElementAttributes, container: Container): Canvas{
        const can = container.document.createElement("canvas");
        container.parentElement.appendChild(can);

        const canvas: Canvas = new Canvas(initialAttributes, can);
        can.className = initialAttributes.className ?? '';

        return canvas;
    }
}