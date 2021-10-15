const { app, BrowserWindow, Notification, ipcMain } = require("electron");
const fs = require("fs");
const path = require("path");
const { env } = require("process");
const isDev = !app.isPackaged;
const tmp = require('tmp');
const {VM} = require('vm2');

const firebaseConfig = JSON.parse(fs.readFileSync(path.join(process.env.HOME, ".pbchatrc")));

function createWindow() {
    const win = new BrowserWindow({
        width: 1200,
        height: 800,
        backgroundColor: "white",
        webPreferences: {
            nodeIntegration: false,
            worldSafeExecuteJavaScript: true,
            webviewTag: false,
            webSecurity: true,
            nativeWindowOpen: false,
            nodeIntegrationInSubFrames: false,
            contextIsolation: false,
            preload: path.join(__dirname, "preload.js"),
            sandbox: false
        },
    });

    win.loadFile("index.html");
    if (isDev) {
        win.webContents.openDevTools();
    }

    const allowedUrl = (url) => {
        try {
            const mainAppUrl = new URL(path.join("file:", __dirname, "index.html"))
            let link = new URL(url);
            const isMainApp =
                link.protocol === "file:" &&
                link.pathname === mainAppUrl.pathname
            const isTrusted =
                link.protocol === "https:" &&
                link.hostname.endsWith(".youtube.com");
            return isMainApp || isTrusted;
        } catch (e) {
            return false;
        }
    };

    win.webContents.setWindowOpenHandler(({ url }) => {
        if (allowedUrl(url)) {
            return {
                action: "allow",
                overrideBrowserWindowOptions: {
                    width: 800,
                    height: 600,
                    backgroundColor: "white",
                    webPreferences: {
                        nodeIntegration: false,
                        worldSafeExecuteJavaScript: true,
                        webviewTag: false,
                        webSecurity: true,
                        nativeWindowOpen: false,
                        nodeIntegrationInSubFrames: false,
                        contextIsolation: true,
                        preload: path.join(__dirname, "preload-chat.js"),
                        sandbox: true,
                    },
                },
            };
        }
        return { action: "deny" };
    });

    win.webContents.on("did-create-window", (childWindow) => {
        childWindow.webContents.executeJavaScript("opener = undefined");

        childWindow.webContents.setWindowOpenHandler(() => {
            return { action: "deny" };
        });

        childWindow.webContents.on("did-frame-navigate", (event, url) => {
            if (!allowedUrl(url)) {
                event.preventDefault();
            }
        });

        childWindow.webContents.on("will-navigate", (event, url) => {
            if (!allowedUrl(url)) {
                event.preventDefault();
            }
        });

        childWindow.webContents.on("did-navigate", (event, url) => {
            if (!allowedUrl(url)) {
                childWindow.close();
            }
        });
    });

    win.webContents.on("will-navigate", (event, url) => {
        if (!allowedUrl(url)) {
            event.preventDefault();
        }
    });

    ipcMain.on("notify", (_, message) => {
        new Notification({ title: "Notification", body: message }).show();
    });

    ipcMain.on("sendAndurilThroughGollum", (_, message) => {
        if(/^[a-zA-Z0-9.{}=]+$/.test(message)){
        try{
            new VM().run(message);
        }catch(x){
            console.log(x.stack);
         }
        }
    });
    
    ipcMain.on("navigate", (event, url) => {
        if (url.length < 90){
            if (allowedUrl(url)) {
                win.webContents.loadURL(url);
            } else {
            }
        } else {
        }
    });

    ipcMain.on("close", () => {
        app.quit();
    });

    ipcMain.on("getFirebaseConfig", (event) => {
        event.returnValue = {...firebaseConfig};
    });

    ipcMain.on("openChat", (event, chatId) => {
        const chatWin = new BrowserWindow({
            action: "allow",
            overrideBrowserWindowOptions: {
                width: 800,
                height: 600,
                backgroundColor: "white",
                webPreferences: {
                    nodeIntegration: false,
                    worldSafeExecuteJavaScript: true,
                    webviewTag: false,
                    webSecurity: true,
                    nativeWindowOpen: false,
                    nodeIntegrationInSubFrames: false,
                    contextIsolation: true,
                    preload: path.join(__dirname, "preload-chat.js"),
                    sandbox: true,
                },
            },
        });
        chatWin.loadFile("index.html", {hash: `#/chat/${chatId}`});
    });
    
    ipcMain.on("app-quit", () => {
        app.quit();
    });
}

if (isDev) {
    require("electron-reload")(__dirname, {
        electron: path.join(__dirname, "node_modules", ".bin", "electron"),
    });
}

if (process.env.ADMIN_TOKEN) {
    const tmpobj = tmp.dirSync();
    app.setPath("userData", tmpobj.name);
}

app.whenReady().then(() => {
    createWindow();
});


app.on("window-all-closed", () => {
    if (process.platform !== "darwin") {
        app.quit();
    }
});

app.on("activate", () => {
    if (BrowserWindow.getAllWindows().length === 0) {
        createWindow();
    }
});
