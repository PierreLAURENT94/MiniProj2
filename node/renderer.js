const ipc = require("electron").ipcRenderer;
/*
function B_ici_click(){
    ipc.send('asynchronous-message', 'ping');
}

document.getElementById("B_ici").addEventListener("click", B_ici_click);
*/
function Reponse(event, arg){
    const message = "Asynchronous message reply: ${arg}";
    //document.getElementById("B_ici").innerText = message;
}

ipc.on("asynchronous-reply", Reponse); 

// --------------------

function Scanner(){
    document.getElementById("scanner").style.display = "none";
    document.getElementById("scannerAtt").style.display = "inline-block";
    alert("scan");
}
document.getElementById("scanner").addEventListener("click", Scanner);

function GitHub(){
    ipc.send('GitHub', 'click');
}
document.getElementById("gitHub").addEventListener("click", GitHub);