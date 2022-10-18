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

    ipc.on("print-data", (event, meteoStationInfo, data) => {
        ["name", "manufacturer", "path", "serialNumber", "version"].forEach(id => {
            document.getElementById(id).innerText = meteoStationInfo[id];
        });
    })
}

if (currentMode === "config") {
    let monitor = document.getElementById("consoleContent");
    let connected = false;
    const scrollDownMonitor = () => {
        monitor.scrollIntoView({
            behavior: "auto",
            block: "end"
        });
    };

    scrollDownMonitor();

    let commandList = [
        {
            name: "LOG_INTERVALL",
            id: 0,
            defaultValue: 10,
            unite: "min",
            interval: [1, 1440]
        }, {
            name: "FILE_MAX_SIZE",
            id: "2",
            defaultValue: 2048,
            unite: "octets",
            interval: [256, 32767]//minimum à modif
        }, {
            name: "TIMEOUT",
            id: "4",
            defaultValue: 30,
            unite: "s",
            interval: [1, 32768]
        }, {
            name: "LUMIN",
            id: "6",
            defaultValue: 1,
            unite: "",
            interval: [0, 1]
        }, {
            name: "LUMIN_LOW",
            id: "8",
            defaultValue: 255,
            unite: "lx",
            interval: [0, "HIGH"]
        }, {
            name: "LUMIN_HIGH",
            id: "10",
            defaultValue: 768,
            unite: "lx",
            interval: ["LOW", 1023]
        }, {
            name: "TEMP_AIR",
            id: "12",
            defaultValue: 1,
            unite: "",
            interval: [0, 1]
        }, {
            name: "MIN_TEMP_AIR",
            id: "14",
            defaultValue: -10,
            unite: "°C",
            interval: [-40, "MAX"]
        }, {
            name: "MAX_TEMP_AIR",
            id: "16",
            defaultValue: 60,
            unite: "",
            interval: ["MIN", 85]
        }, {
            name: "HYGR",
            id: "18",
            defaultValue: 1,
            unite: "",
            interval: [0, 1]
        }, {
            name: "HYGR_MINT",
            id: "20",
            defaultValue: 0,
            unite: "°C",
            interval: [-40, "MAX"]
        }, {
            name: "HYGR_MAXT",
            id: "22",
            defaultValue: 50,
            unite: "°C",
            interval: ["MIN", 85]
        }, {
            name: "PRESSURE",
            id: "24",
            defaultValue: 1,
            unite: "",
            interval: [0, 1]
        }, {
            name: "PRESSURE_MIN",
            id: "26",
            defaultValue: 850,
            unite: "hPa",
            interval: [300, "MAX"]
        }, {
            name: "PRESSURE_MAX",
            id: "28",
            defaultValue: 1080,
            unite: "hPa",
            interval: ["MIN", 1100]
        }
    ];

    const getCommand = (commandName) => commandList.filter(x => x.name.toLowerCase() === commandName.toLowerCase())[0];
    const getCommandById = (commandId) => commandList.filter(x => x.id === commandId)[0];

    const parseSettings = (settingsList) => {
        let result = [];
        settingsList.split(" ").forEach(setting => {
            let temp = setting.split("=");
            let commandInfos = getCommandById(temp[0]);
            if (commandInfos) result.push(`${commandInfos.name} = ${temp[1]}${commandInfos.unite.length > 0 ? ` (${commandInfos.unite})` : ""}`);
        });
        return result;
    };

    const newOutput = (msg, type) => {
        let newResponse = document.createElement("p");
        if (type) newResponse.classList.add(type);
        newResponse.innerText = msg;
        monitor.appendChild(newResponse);
        scrollDownMonitor();
    }

    const errorOutput = (errorMsgList) => {
        errorMsgList.splice(0, 0, "Erreur :");
        errorMsgList[errorMsgList.length - 1] += ".";
        errorMsgList.forEach(x => {
            newOutput(x, "error");
        });
    }

    document.getElementById("commandInput").addEventListener("keyup", (event) => {
        if (event.key === "Enter") {
            let command = event.target.value;
            if (!connected) return errorOutput(["Le module météo n'est pas connecté"]);
            event.target.value = "";
            newOutput(command, "pc");

            let commandArgs = command.split(/ +/);

            for (let i = 0; i < commandArgs.length; i++) {
                let commandName = commandArgs[i].split("=")[0] || null;
                let commandValue = commandArgs[i].split("=")[1] || null;
                let commandInfos = getCommand(commandName);

                if (commandName) {
                    commandName = commandName.toLowerCase();
                    newOutput(commandArgs[i].toUpperCase(), "command");
                }

                if (commandName === "reset") {
                    newOutput("Reset des valeurs effectué.", "arduino");
                }
                else if (commandName === "version") {
                    ipc.send("command", "version");
                }
                else if (commandName === "clock") {
                    let argsForClock = commandValue ? commandValue.split(":") : [];
                    if (argsForClock.length !== 3) errorOutput([`Nombre d'arguments invalides : "${argsForClock.length}" au lieu de "3"`, `Format attendu : "CLOCK=HH:MM:SS"`])
                    else if (isNaN(argsForClock[0])) errorOutput([`Type invalide : "${argsForClock[0]}" n'est pas un nombre`])
                    else if (isNaN(argsForClock[1])) errorOutput([`Type invalide : "${argsForClock[1]}" n'est pas un nombre`])
                    else if (isNaN(argsForClock[2])) errorOutput([`Type invalide : "${argsForClock[2]}" n'est pas un nombre`])
                    else {
                        argsForClock[0] = parseInt(argsForClock[0]);
                        argsForClock[1] = parseInt(argsForClock[1]);
                        argsForClock[2] = parseInt(argsForClock[2]);

                        if (argsForClock[0] < 0 || argsForClock[0] > 23) errorOutput([`Argument d'heure invalide : "${argsForClock[0]}" n'est pas compris entre "0" et "23"`])
                        else if (argsForClock[1] < 0 || argsForClock[1] > 59) errorOutput([`Argument de minute invalide : "${argsForClock[1]}" n'est pas compris entre "0" et "59"`])
                        else if (argsForClock[2] < 0 || argsForClock[2] > 59) errorOutput([`Argument de seconde invalide : "${argsForClock[2]}" n'est pas compris entre "0" et "59"`])
                        else {
                            newOutput(`Changement de l'heure effectué. Nouvelle heure : ${argsForClock[0]}h${argsForClock[1]} et ${argsForClock[2]}s`, "arduino");
                        }
                    }
                }
                else if (commandName === "date") {
                    let argsForDate = commandValue ? commandValue.split(",") : [];
                    if (argsForDate.length !== 3) errorOutput([`Nombre d'arguments invalides : "${argsForDate.length}" au lieu de "3"`, `Format attendu : "DATE=MOIS,JOUR,ANNEE"`])
                    else if (isNaN(argsForDate[0])) errorOutput([`Type invalide : "${argsForDate[0]}" n'est pas un nombre`])
                    else if (isNaN(argsForDate[1])) errorOutput([`Type invalide : "${argsForDate[1]}" n'est pas un nombre`])
                    else if (isNaN(argsForDate[2])) errorOutput([`Type invalide : "${argsForDate[2]}" n'est pas un nombre`])
                    else {
                        argsForDate[0] = parseInt(argsForDate[0]);
                        argsForDate[1] = parseInt(argsForDate[1]);
                        argsForDate[2] = parseInt(argsForDate[2]);

                        if (argsForDate[0] < 1 || argsForDate[0] > 12) errorOutput([`Argument d'heure invalide : "${argsForDate[0]}" n'est pas compris entre "1" et "12"`])
                        else if (argsForDate[1] < 1 || argsForDate[1] > 31) errorOutput([`Argument de minute invalide : "${argsForDate[1]}" n'est pas compris entre "1" et "31"`])
                        else if (argsForDate[2] < 2000 || argsForDate[2] > 2099) errorOutput([`Argument de seconde invalide : "${argsForDate[2]}" n'est pas compris entre "2000" et "2099"`])
                        else {
                            let date = new Date(`${argsForDate[1]}/${argsForDate[0]}/${argsForDate[2]}`);
                            newOutput(`Changement de date effectué. Nouvelle date : ${new Intl.DateTimeFormat("fr-FR", { weekday: 'long', day: 'numeric', month: 'long', year: 'numeric' }).format(date)} `, "arduino");
                        }
                    }
                }
                else if (!commandInfos) {
                    errorOutput([`"${commandName}" nom de commande invalide`]);
                }
                else {
                    if (isNaN(commandValue)) errorOutput([`Type invalide : "${commandValue}" n'est pas un nombre`]);
                    else {
                        //à modif par currentValue
                        let low = isNaN(commandInfos.interval[0]) ? getCommand(commandInfos.name.replace(commandInfos.interval[0] === "MIN" ? "MAX" : "HIGH", commandInfos.interval[0] === "MIN" ? "MIN" : "LOW")).defaultValue : commandInfos.interval[0];
                        let high = isNaN(commandInfos.interval[1]) ? getCommand(commandInfos.name.replace(commandInfos.interval[1] === "MAX" ? "MIN" : "LOW", commandInfos.interval[1] === "MAX" ? "MAX" : "HIGH")).defaultValue : commandInfos.interval[1];
                        if (commandValue < low || commandValue > high) errorOutput([`Argument invalide : "${commandValue}" n'est pas compris entre "${low}" et "${high}"`])
                        else {
                            newOutput(`Nouvelle valeur affectée au paramètre "${commandInfos.name}" : ${commandValue}`, "arduino");
                            //ipc.send("command", commandArgs[i]);
                        }
                    }
                }
            }

        };
    });

    ipc.once("arduino-connected", (event) => {
        newOutput("Module météo connecté.", "pc");
        newOutput("Merci de ne pas débrancher le cable lors de l'exécution d'une commande.", "pc");
        newOutput("Récupération des paramètres actuels...", "pc");
    });

    ipc.on("current-settings", (event, settings) => {

        newOutput("Paramètres actuels :", "arduino");

        parseSettings(settings).forEach(x => {
            newOutput(x);
        });

        newOutput("En attente d'instructions...", "arduino");

        if (!connected) connected = true;
    });

    ipc.on("answer", (event, msg) => {
        newOutput(msg, "arduino");
    });

};//à modif Placer les valeurs actuels dans les variables et faire une boucle pour attendre la fin d'une commande pour lancer la suivante