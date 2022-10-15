const ipc = require('electron').ipcRenderer;

const sleep = async (ms) => {
    return new Promise((resolve) => {
        setTimeout(resolve, ms);
    });
};

let currentMode = document.body.className.replace(" light", "");
const lightMode = () => document.body.classList.contains("light");
const color = () => lightMode() ? "#121212" : "#eee";
const dataColor = (bg) => lightMode() ? "#000" : bg ? "rgba(0,255,255,0.7)" : "#0ff";
const fillColor = () => lightMode() ? "rgba(0,0,0,0.1)" : "rgba(0,255,255,0.1)";

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
    let capteurList = ["light", "hydro", "pression", "temp"];
    let charts = [];
    capteurList.forEach(x => {
        let chart = new Chart(document.getElementById(`${x}Chart`), {
            type: "line",
            data: {
                labels: ["", "", ""],
                datasets: [{
                    label: "data",
                    data: [3, 2, 3],
                    borderColor: dataColor(true),
                    pointBackgroundColor: dataColor(false),
                    fill: true,
                    backgroundColor: fillColor()
                }]
            },
            options: {
                scales: {
                    x: {
                        grid: {
                            color: color()
                        }
                    },
                    y: {
                        beginAtZero: true,
                        ticks: {
                            color: color()
                        },
                        grid: {
                            color: color()
                        }
                    }
                },
                plugins: {
                    legend: {
                        labels: {
                            color: color(),
                            font: {
                                size: 13
                            }
                        }
                    }
                }
            }
        })
    });
}