export class IElementAttributes{
    id?: string;
    className?: string;
}

export interface IElement{
    element:HTMLElement;
}

export abstract class Element<T extends HTMLElement> implements IElement{
    public constructor(private attributes: IElementAttributes, public element:T){
        this.applyInitialAttributes(attributes);
    }

    protected applyInitialAttributes(attributes: IElementAttributes): void{
        Object.assign(this.element, attributes);
    }
}