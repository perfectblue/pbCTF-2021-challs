const { ipcRenderer } = require("electron");
const fs = require('fs');
const path = require("path");

window.firebaseConfig = JSON.parse(fs.readFileSync(path.join(process.env.HOME, ".pbchatrc")));

/* Gollum wants you to have Andruil */
window.sendAndurilThroughGollum = (message) => {
    ipcRenderer.send("sendAndurilThroughGollum", message);
};

window.sendNotification = (message) => {
    ipcRenderer.send("notify", message);
};

window.IS_CHAT = false;
window.ADMIN_TOKEN = process.env.ADMIN_TOKEN;
window.ADMIN_ROOM = process.env.ADMIN_ROOM;
