type RGB = { r: number; g: number; b: number };

function SetBackground_(color?: string | RGB | null): void {
    if (!color)
        console.warn("No color, defaulting to black");

    try {
        let new_: string;

        if (typeof color === "object" && color !== null && !Array.isArray(color)) {
            const rgb = color as RGB;
            const hex = "#" + [rgb.r, rgb.g, rgb.b]
                .map(x => x.toString(16).padStart(2, '0'))
                .join('')
                .toUpperCase();

            new_ = hex;
        } else if (color !== null && color !== undefined) {
            new_ = color;
        } else {
            new_ = "black";
        }

        const html_ = document.querySelector("html");
        if (html_) {
            html_.style.width = "100%";
            html_.style.height = "100%";
            html_.style.position = "absolute";
            html_.style.top = "0";
            html_.style.left = "0";
            html_.style.backgroundColor = new_ ?? "#000000";
        }
    } catch {
        // do absolutely nothing
    }
}

export { SetBackground_ };
