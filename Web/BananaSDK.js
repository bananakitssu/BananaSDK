function SetBackground_ (color) {
    if (!color)
        console.warn("No color, defaulting to black");
    
    try {
        let new_;
        
        if (typeof color === "object" && color !== null && !Array.isArray(color)) {
            const hex = "#" + [rgb.r, rgb.g, rgb.b]
                .map(x => x.toString(16).padStart(2, '0'))
                .join('')
                .toUpperCase();
                
            new_ = hex;
        } else if (color !== null) {
            new_ = color;
        } else {
            new_ = "black";
        }
        
        const html_ = document.querySelector("html");
        html_.style.width = "100%";
        html_.style.height = "100%";
        html_.style.position = "absolute";
        html_.style.top = "0";
        html_.style.left = "0";
        html_.style.backgroundColor = new_ != null ? new_ : "#000000";
    }
}

export { SetBackground_ };
