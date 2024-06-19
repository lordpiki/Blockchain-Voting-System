const circuitCanvas = document.getElementById('circuitCanvas');
const ctx = circuitCanvas.getContext('2d');

const dotSize = 5;
const gridSize = 30;
const spacing = 60;
const dotProbability = 0.5; // Probability to draw a dot
const connectionProbability = 0.4;
const diagonalProbability = 0.1;
const removeIsolatedProbability = 1; // Probability to remove isolated dots

circuitCanvas.width = window.innerWidth;
circuitCanvas.height = window.innerHeight;

let gridPoints = [];

function drawBackground() {
    ctx.fillStyle = 'black';
    ctx.fillRect(0, 0, circuitCanvas.width, circuitCanvas.height);
}

function calcGrid() {

    for(let x = 0; x < circuitCanvas.width; x += spacing) {
        for(let y = 0; y < circuitCanvas.height; y += spacing) {
            let isBlack = false;
            if (Math.random() < dotProbability) {
                isBlack = true;
            }

            const point = {x, y, isBlack: isBlack};
            gridPoints.push(point);
        }
    }

}

function drawConnections() {
    ctx.strokeStyle = 'green';
    ctx.lineWidth = 2;
    ctx.shadowColor = 'lime';
    ctx.globalAlpha = 0.8;

    for(let i = 0; i < gridPoints.length; i++) {
        const {x, y} = gridPoints[i];
        
        if(Math.random() < connectionProbability && x + spacing < circuitCanvas.width) {
            const nextPoint = gridPoints.find(p => p.x === x + spacing && p.y === y);
            if(nextPoint) {
                ctx.beginPath();
                ctx.moveTo(x, y);
                ctx.lineTo(x + spacing, y);
                ctx.stroke();
                ctx.closePath();
            }
        }
        
        if(Math.random() < connectionProbability && y + spacing < circuitCanvas.height) {
            const nextPoint = gridPoints.find(p => p.x === x && p.y === y + spacing);
            if(nextPoint) {
                ctx.beginPath();
                ctx.moveTo(x, y);
                ctx.lineTo(x, y + spacing);
                ctx.stroke();
                ctx.closePath();
            }
        }
    }
    
    for(let i = 0; i < gridPoints.length; i++) {
        const {x, y} = gridPoints[i];
        
        if(Math.random() < diagonalProbability && x + spacing < circuitCanvas.width && y + spacing < circuitCanvas.height) {
            const nextPoint = gridPoints.find(p => p.x === x + spacing && p.y === y + spacing);
            if(nextPoint) {
                ctx.beginPath();
                ctx.moveTo(x, y);
                ctx.lineTo(x + spacing, y + spacing);
                ctx.stroke();
                ctx.closePath();
            }
        }
        
        if(Math.random() < diagonalProbability && x - spacing >= 0 && y + spacing < circuitCanvas.height) {
            const nextPoint = gridPoints.find(p => p.x === x - spacing && p.y === y + spacing);
            if(nextPoint) {
                ctx.beginPath();
                ctx.moveTo(x, y);
                ctx.lineTo(x - spacing, y + spacing);
                ctx.stroke();
                ctx.closePath();
            }
        }
    }
}

function drawGrid() {
    ctx.globalAlpha = 0.8;

    
    gridPoints.forEach(point => {


        if(!point.isBlack) {
            drawDot(point);
    }
    });

}

function drawDot(point, shadow='lime') {
    ctx.fillStyle = 'lime';
    ctx.shadowColor = shadow;
    ctx.shadowBlur = 10;
    
    ctx.beginPath();
    ctx.arc(point.x, point.y, dotSize, 0, Math.PI * 2);
    ctx.fill();
    ctx.closePath();
    ctx.fillStyle = 'black';
    ctx.shadowColor = 'black';
    ctx.shadowBlur = 0;
    ctx.beginPath();
    ctx.arc(point.x, point.y, dotSize * 0.7, 0, Math.PI * 2);
    ctx.fill();
    ctx.closePath();
}

function animateDots() {
    setInterval(() => {
        gridPoints.forEach(point => {
            if (!point.isBlack) {
                drawDot(point);
                setTimeout(() => {
                    drawDot(point, 'black');
                }, 2000);
            }
        });
    }, 1000);

}

function draw() {
    drawBackground();
    calcGrid();
    drawConnections();
    drawGrid();
    // animateDots();
    // removeIsolatedDots(); // Remove isolated dots after drawing connections
}

draw();
