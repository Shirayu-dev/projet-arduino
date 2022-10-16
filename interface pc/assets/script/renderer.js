const ipc = require('electron').ipcRenderer;

const sleep = async (ms) => {
    return new Promise((resolve) => {
        setTimeout(resolve, ms);
    });
};

let currentMode = document.body.className.replace(" light", "");
const gridColor = "#e8e8e8";
const textColor = "#000";
const transparentColor = "rgba(0,0,0,0)";
const fillColor = ["#5b94fb88", "#24cbdd88", "#55dc7d88", "#ff000088"];

if (currentMode === "wait") {
    let message = document.getElementById("msg");
    let messageEffect = document.getElementById("msg-points");
    setInterval(() => {
        if (messageEffect.innerText === "...") {
            messageEffect.innerText = "";
        }
        else {
            messageEffect.innerText += ".";
        }
    }, 1000);

    ipc.on("connected", (event) => {
        message.innerText = "Module météo détécté. En attente d'échange de données";
    });

    ipc.on("disconnected", (event) => {
        message.innerText = "En attente d'une connection avec le module météo";
    })
};
if (currentMode === "mtnc") {
    const chart = new Chart(document.getElementById(`globalChart`), {
        type: "line",
        data: {
            labels: ["", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", ""],
            datasets: [{
                label: "Luminosité",
                data: [0, 0, 0, 0, 0, 0, 0, 0, 8, 12, 8, 7, 6, 12, 10, 11, 8, 7, 6, 5, 4, 6, 8, 7, 10],
                borderColor: transparentColor,
                pointBackgroundColor: transparentColor,
                fill: true,
                backgroundColor: fillColor[0],
                tension: 0.4
            },
            {
                label: "Humidité",
                data: [0, 0, 0, 0, 0, 0, 0, 0, 2, 3, 5, 8, 2, 5, 4, 3, 5, 12, 10, 11, 12, 8, 5, 4, 3],
                borderColor: transparentColor,
                pointBackgroundColor: transparentColor,
                fill: true,
                backgroundColor: fillColor[1],
                tension: 0.4
            },
            {
                label: "Pression",
                data: [0, 0, 0, 0, 0, 0, 0, 0, 5, 8, 2, 3, 5, 5, 4, 8, 6, 11, 10, 9, 8, 8, 7, 5, 2],
                borderColor: transparentColor,
                pointBackgroundColor: transparentColor,
                fill: true,
                backgroundColor: fillColor[2],
                tension: 0.4
            },
            {
                label: "Température",
                data: [0, 0, 0, 0, 0, 0, 0, 0, 2, 3, 3, 4, 5, 4, 4, 5, 4, 3, 4, 3, 4, 4, 5, 5, 3],
                borderColor: transparentColor,
                pointBackgroundColor: transparentColor,
                fill: true,
                backgroundColor: fillColor[3],
                tension: 0.4
            }]
        },
        options: {
            scales: {
                x: {
                    grid: {
                        color: gridColor
                    }
                },
                y: {
                    beginAtZero: true,
                    ticks: {
                        color: textColor
                    },
                    grid: {
                        color: transparentColor
                    }
                }
            },
            plugins: {
                legend: {
                    labels: {
                        color: textColor,
                        font: {
                            size: 13
                        }
                    }
                }
            }
        }
    });
}