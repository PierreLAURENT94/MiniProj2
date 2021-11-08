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

ipc.on("Scanner", ScannerOK); 

ipc.on("Cartes", Cartes); 

ipc.on("ATRConnecte", ATRConnecte); 

// --------------------
var cartes;
function Cartes(event, arg){
    cartes = arg;
    var genHTML = "";
    for(var tour = 0; tour < cartes.length; tour++){
        genHTML += `
        <input type="checkbox" id="collapse-section` + (tour + 1) + `" checked aria-hidden="true">
        <label for="collapse-section1" aria-hidden="true">` + cartes[tour][0] + ` (` + tour + `)</label>
        <div>
            <p>ATR: ` + cartes[tour][1] + `<button class="secondary suppCarte">Supprimer</button></p>
        </div>
        `
    }

    document.getElementById("cartes").innerHTML = genHTML;
}
ipc.send('Cartes', "");

ipc.send('ATRConnecte', "");
function ATRConnecte(event, arg){
    // ici
}

function Scanner(){
    document.getElementById("scanner").style.display = "none";
    document.getElementById("scannerAtt").style.display = "inline-block";
    ipc.send('Scanner', document.getElementById("nomCarte").value);
    
}
document.getElementById("scanner").addEventListener("click", Scanner);

function ScannerOK(){
    document.getElementById("scanner").style.display = "block";
    document.getElementById("scannerAtt").style.display = "none";
    document.getElementById("nomCarte").value = "";
    ipc.send('Cartes', "");
}

function GitHub(){
    ipc.send('GitHub', 'click');
}
document.getElementById("gitHub").addEventListener("click", GitHub);