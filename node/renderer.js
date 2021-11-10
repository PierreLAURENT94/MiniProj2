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
            <p>ATR: ` + cartes[tour][1] + `<button class="secondary suppCarte" id="` + cartes[tour] + `">Supprimer</button></p>
        </div>
        `
    }

    document.getElementById("cartes").innerHTML = genHTML;
    for(tour=0;tour < document.getElementsByClassName("suppCarte").length;tour++){
        //document.getElementsByClassName("suppCarte")[tour]
    }
}
ipc.send('Cartes', "");

ipc.send('ATRConnecte', "");
function ATRConnecte(event, arg){
    console.log(arg);
    var nomConnecte = "? ? ?";
    for(tour=0;tour < cartes.length;tour++){
        console.log(cartes[tour][1]);
        if(cartes[tour][1] == arg){
            nomConnecte = cartes[tour][0];
        }
    }
    console.log(nomConnecte);
    document.getElementById("tspan836").innerHTML = nomConnecte;
    if(nomConnecte == "? ? ?"){
        document.getElementById("rect912").style.fill = "var(--input-invalid-color)";
    }
    else{
        document.getElementById("rect912").style.fill = "var(--carte)";
    }
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