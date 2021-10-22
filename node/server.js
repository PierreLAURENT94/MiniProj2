"use strict";
var http = require("http");
const smartcard = require('smartcard'); // https://github.com/santigimeno/node-pcsclite

const Devices = smartcard.Devices;
const devices = new Devices();
devices.on("device-activated", LecteurConnecte);

var server = http.createServer();
server.on("request", Requete);

var coureurs = [];

function Requete(request, response){
    response.writeHead(200, {"Content-type": "application/json; charset=utf-8"});
    response.write(JSON.stringify(coureurs));
    response.end();
} 
server.listen(80);

function LecteurConnecte(event){
    const currentDevices = event.devices;
    var device = event.device;
    device.on("card-inserted", CarteConnecte);
}

function CarteConnecte(event){
    var carte = event.card;
    console.log("Carte ATR: " + carte.getAtr());
    ATRConnecte(carte.getAtr());
}

function ATRConnecte(ATR){
    var deja = false;
    var numCoureurs;
    for(var tour = 0; tour < coureurs.length; tour++){
        if(ATR == coureurs[tour]){
            deja = true;
            numCoureurs = tour + 1;
        }
    }
    if(!deja){
            coureurs.push(ATR);
            numCoureurs = coureurs.length;
    }
    console.log("Passage du coureur numero: " + numCoureurs);
}
