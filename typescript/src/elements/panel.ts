import { Container } from "./container";
import { Element, IElementAttributes } from "./element";

export class Panel extends Element<HTMLDivElement>{
    private constructor(private initialAttributes: IElementAttributes, public element: HTMLDivElement){
        super(initialAttributes, element);
    }

    public static createInContainer(initialAttributes: IElementAttributes, container: Container): Panel{
        const div = container.document.createElement("div");
        container.parentElement.appendChild(div);

        const canvas: Panel = new Panel(initialAttributes, div);
        div.className = initialAttributes.className ?? '';

        return canvas;
    }
}