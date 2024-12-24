import { IElement } from "./element";

export class Container{

    public document: Document;
    public parentElement: Node;
    private elements: IElement[] = [];

    public constructor(node?: Node){
        this.document = document;

        this.parentElement = document.body;
        if(node){
            this.parentElement = node;
        }
    }

    public addElement(element: IElement): void{
        this.parentElement.appendChild(element.element);
        this.elements.push(element);
    }
}