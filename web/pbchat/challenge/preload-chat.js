const { ipcRenderer, contextBridge } = require("electron");

contextBridge.exposeInMainWorld("electron", {
    api: {
        sendNotification(message) {
            ipcRenderer.send("notify", message);
        },
        navigate(url) {
            ipcRenderer.send("navigate", url);
        },
        close() {
            ipcRenderer.send("close");
        },
        openChat(chatId) {
            ipcRenderer.send("openChat", chatId);
        },
        getFirebaseConfig() {
            return ipcRenderer.sendSync("getFirebaseConfig");
        },
    },
});

contextBridge.exposeInMainWorld("IS_CHAT", true);
contextBridge.exposeInMainWorld("ADMIN_TOKEN", process.env.ADMIN_TOKEN);
contextBridge.exposeInMainWorld("ADMIN_ROOM", process.env.ADMIN_ROOM);