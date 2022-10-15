const { app, BrowserWindow } = require('electron');
app.commandLine.appendSwitch("disable-renderer-backgrounding");
const path = require('path');
const ipc = require('electron').ipcMain;
const devTools = true;
const detectDisconnect = 3000;
const timeBetweenCheckPort = 1000;

const EventEmitter = require("events");
class AnEmitter extends EventEmitter { };
const windowEmitter = new AnEmitter();

const sleep = async (ms) => {
	return new Promise((resolve) => {
		setTimeout(resolve, ms);
	});
};

const getFile = fileName => `assets/html/${fileName}`;

//-------------------------------------------------createWindow-------------------------------------------------

let currentMode = "wait";

async function createWindow() {
	const mainWindow = new BrowserWindow({
		icon: path.join(__dirname, "/assets/img/logo_database.png"),
		width: 1890,
		height: 1080,
		resizable: false,
		autoHideMenuBar: true,
		webPreferences: {
			nodeIntegration: true,
			contextIsolation: false,
			devTools: devTools
		}
	});

	windowEmitter.on("mtnc", async (event, data) => {
		if (currentMode !== "mtnc") {
			currentMode = "mtnc";
			await mainWindow.loadFile(getFile("mtnc.html"));
		}
		if (event === "print-data") return;//console.log(data);
		//mainWindow.webContents.send(event, data);
	});

	windowEmitter.on("no-data", async () => {
		if (currentMode !== "wait") {
			await mainWindow.loadFile(getFile("wait.html"));
			currentMode = "wait";
		};
		mainWindow.webContents.send("connected");
	});

	windowEmitter.on("disconnected", async () => {
		if (currentMode !== "wait") {
			await mainWindow.loadFile(getFile("wait.html"));
			currentMode = "wait";
		} else mainWindow.webContents.send("disconnected");
	});

	mainWindow.loadFile(getFile("mtnc.html"));
};

//-------------------------------------------------Lancement de l'app-------------------------------------------------

app.whenReady().then(() => {
	createWindow();

	app.on('activate', function () {
		if (BrowserWindow.getAllWindows().length === 0) createWindow();
	});
});

app.on('window-all-closed', function () {
	if (process.platform !== 'darwin') app.quit();
});

//---------------------------------------Initialisation port série----------------------------------------------------
/*
const serialPort = require("serialport");
const Readline = serialPort.ReadlineParser;
let timeout;

const checkPort = () => {
	serialPort.SerialPort.list().then(ports => {
		let portInfos = ports.filter(p => p.manufacturer.startsWith("Arduino Srl"))[0];
		if (!portInfos) return;
		portConnected();
		let portPath = portInfos.path;
		const port = new serialPort.SerialPort(
			{
				path: `${portPath}`,
				baudRate: 9600,
				dataBits: 8,
				parity: 'none',
				stopBits: 1,
				flowControl: false
			}
		).setEncoding("utf-8");
		port.on("close", portDisconnected);

		const parser = new Readline("\n");
		port.pipe(parser);

		parser.on("data", dataReceive);
	});
};

let checkPortInterval = setInterval(checkPort, timeBetweenCheckPort);

const portConnected = () => {
	clearInterval(checkPortInterval);
	windowEmitter.emit("no-data");
};


const dataReceive = (rawData) => {
	try {
		let data = JSON.parse(rawData);
		if (data.mode) {
			if (timeout) clearTimeout(timeout);
			timeout = setTimeout(() => {
				windowEmitter.emit("no-data");
			}, detectDisconnect);
		}
		if (data.mode === "mntc") return windowEmitter.emit("mtnc", "print-data", data);
	}
	catch {
		console.log("Erreur lors de la lecture des données.");
	}
};

const portDisconnected = () => {
	windowEmitter.emit("disconnected");
	checkPortInterval = setInterval(checkPort, timeBetweenCheckPort);
};

const sendDataToArduino = (data, port) => {
	Array.from(data).forEach((c) => {
		port.write(Buffer.from(c, "ascii"), err => {
			if (err) console.log(`Erreur : ${err}`);
		});
	});
};
*/