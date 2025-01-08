const { Jimp } = require("jimp");

async function convertImage() {
    const image = await Jimp.read('./snake.png');

    // deixando (tentando deixar) branco transparente
    image.scan(0, 0, image.bitmap.width, image.bitmap.height, (x, y, idx) => {
        const red = image.bitmap.data[idx + 0];
        const green = image.bitmap.data[idx + 1];
        const blue = image.bitmap.data[idx + 2];

        //if (red === 255 && green === 255 && blue === 255) {
        if (red >= 200 && green >= 200 && blue >= 200) {
            image.bitmap.data[idx + 3] = 0;
        }
    });

    const rgbaData = [];
    image.scan(0, 0, image.bitmap.width, image.bitmap.height, (x, y, idx) => {
        rgbaData.push(image.bitmap.data[idx + 0]); // R
        rgbaData.push(image.bitmap.data[idx + 1]); // G
        rgbaData.push(image.bitmap.data[idx + 2]); // B
        rgbaData.push(image.bitmap.data[idx + 3]); // A
    });

    const headerContent = 
`#pragma once

namespace assets {

    constexpr size_t SNAKE_HEAD_WIDTH = ${image.bitmap.width};
    constexpr size_t SNAKE_HEAD_HEIGHT = ${image.bitmap.height};

    constexpr unsigned char SNAKE_HEAD_DATA[] = {
        ${rgbaData.map(byte => `0x${byte.toString(16).padStart(2, '0')}`).join(', ')}
    };
}`;

    console.log(headerContent);
}

convertImage();