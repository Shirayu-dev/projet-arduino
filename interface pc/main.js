const { app, BrowserWindow } = require('electron');
app.commandLine.appendSwitch("disable-renderer-backgrounding");
const path = require('path');
const ipc = require('electron').ipcMain;
const devTools = true;

const EventEmitter = require("events");
class AnEmitter extends EventEmitter { };
const windowEmitter = new AnEmitter();

const sleep = async (ms) => {
	return new Promise((resolve) => {
		setTimeout(resolve, ms);
	});
};

//-------------------------------------------------createWindow-------------------------------------------------

async function createWindow() {
	const mainWindow = new BrowserWindow({
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

	windowEmitter.on("ipc-send-window", (event, data) => {
		if (event === "print-mntc") return console.log(data);
		//mainWindow.webContents.send(event, data);
	});

	mainWindow.loadFile('wait.html');
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

const SerialPort = require("serialport");
const Readline = SerialPort.ReadlineParser;
const portPath = "COM3";
SerialPort.SerialPort

const sendDataToArduino = (data, port) => {
	Array.from(data).forEach((c) => {
		port.write(Buffer.from(c, "ascii"), err => {
			if (err) console.log(`Erreur : ${err}`);
		});
	});
};

const port = new SerialPort.SerialPort(
	{
		path: `${portPath}`,
		baudRate: 9600,
		dataBits: 8,
		parity: 'none',
		stopBits: 1,
		flowControl: false
	}
).setEncoding("utf-8");

const parser = new Readline("\n");
port.pipe(parser);

parser.on("data", async (rawData) => {
	let data = JSON.parse(rawData);
	if (data.mode === "mntc") return windowEmitter.emit("mtnc", "print-data", data);
});