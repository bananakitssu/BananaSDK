export SetBackground_ (color) {
    if (!color)
        console.warn("No color, defaulting to black");
    
    try {
        let new_ = "";
        
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
        html_.style.backgroundColor = new_;
    }
}
