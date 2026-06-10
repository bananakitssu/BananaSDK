import { useState, useEffect } from 'react';

export default Button ({text} : {text: string}) {
    const [isHovered, setIsHovered] = useState(false);
    
    return (
        <button style={{
            border: 'none',
            background: isHovered ? 'grey' : 'darkgrey',
            transition: 'background 0.5s ease-in-out'
        }}
        onMouseEnter{() => { setIsHovered(true) }}
        onMouseLeave{() => { setIsHovered(false) }}>{text}<button>
    )
}
