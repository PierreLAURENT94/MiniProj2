const {app, ipcMain, BrowserWindow} = require("electron");
const http = require("http");
const smartcard = require('smartcard'); // https://github.com/santigimeno/node-pcsclite
const open = require('open');

function createWindow(){
  const mainWindow = new BrowserWindow({
    width: 1350,
    height: 1015,
    webPreferences: {
      nodeIntegration: true,
      contextIsolation: false,
      icon: "logo.ico",
    }
  });
  mainWindow.loadFile("index.html");
  mainWindow.setMenuBarVisibility(false);
  mainWindow.webContents.openDevTools();
}

app.whenReady().then(createWindow);

function FermerApp(){
  app.quit();
}

app.on("window-all-closed", FermerApp);

ipcMain.on('asynchronous-message', (event, arg) => {
  event.sender.send('asynchronous-reply', 'pong');
  console.log("test");

})

ipcMain.on('GitHub', (event, arg) => {
  open("https://github.com/PierreLAURENT94/MiniProj2");
})

var scanner = false;
var nomCarte = "";
var eventScanner = null;
ipcMain.on('Scanner', (event, arg) => {
  scanner = true;
  nomCarte = arg;
  eventScanner = event;
})

ipcMain.on('Cartes', (event, arg) => {
  event.reply('Cartes', cartes);
})

var eventATRConnecte = null;
ipcMain.on('ATRConnecte', (event, arg) => {
  eventATRConnecte = event;
})
// ----------------------------

const Devices = smartcard.Devices;
const devices = new Devices();
devices.on("device-activated", LecteurConnecte);

var nomCourse = "SCHNEIDER ELECTRIC MARATHON DE PARIS 2022";
var logoCourse = "https://upload.wikimedia.org/wikipedia/commons/5/5a/Paris_Marathon_Logo_2013.png";

var server = http.createServer();
server.on("request", Requete);

var cartes = [["Pierre", "3b8f80014545458f666s", 500]]; // [0:[0:nom, 1:atr, 2:temps], 1:[0:nom, 1:atr, 2:temps]]

function Requete(request, response){
    response.writeHead(200, {"Content-type": "text/html; charset=utf-8"});
    response.write(Afficheur());
    response.end();
} 
server.listen(1201);

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
  /*
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
    */
    eventATRConnecte.reply('ATRConnecte', ATR);
    if(scanner){
      cartes.push([nomCarte, ATR, 500]);
      eventScanner.reply('Scanner', 'OK');
      scanner = false;
      nomCarte = "";
      eventScanner = null;
    }
    else{

    }
}

var afficheur = "";
function Afficheur(){
  afficheur = `
<!DOCTYPE html>
<html>
    <head>
      <meta charset="UTF-8"/>
      <meta http-equiv="refresh" content="3"/>
      <link rel="stylesheet" href="https://gitcdn.link/repo/Chalarangelo/mini.css/master/dist/mini-default.min.css"/>
      <title>Course Kronos® Afficheur</title>
    </head>
    <body>
      <table style="overflow: hidden;max-height: none;" class="striped hoverable">
        <caption>` + nomCourse + `</caption>
        <thead>
          <tr>
            <th>Nom</th>
            <th>Temps étapes 1</th>
            <th>Rang étapes 1</th>
            <th>Temps final</th>
            <th>Rang final</th>
          </tr>
        </thead>
        <tbody>
          <tr>
            <td data-label="Nom">Pierre</td>
            <td data-label="Temps étapes 1">12s</td>
            <td data-label="Rang étapes 1">2/3</td>
            <td data-label="Temps final">12s</td>
            <td data-label="Rang final"><mark>2/3</mark></td>
          </tr>
          <tr>
            <td>Vincent</td>
            <td>9s</td>
            <td>1/3</td>
            <td>9s</td>
            <td><mark class="tertiary">1/3</mark></td>
          </tr>
          <tr>
            <td>Jean</td>
            <td>18s</td>
            <td>3/3</td>
            <td>18s</td>
            <td><mark>3/3</mark></td>
          </tr>
          <tr>
            <td data-label="Nom">Retardo</td>
            <td data-label="Temps étapes 1"><div class="spinner" style="margin: 0;width: .75rem;height: .75rem;animation-duration: 3s"></div></td>
            <td data-label="Rang étapes 1"><div class="spinner" style="margin: 0;width: .75rem;height: .75rem;animation-duration: 3s"></div></td>
            <td data-label="Temps final"><div class="spinner" style="margin: 0;width: .75rem;height: .75rem;animation-duration: 3s"></div></td>
            <td data-label="Rang final"><div class="spinner primary" style="margin: 0;width: .75rem;height: .75rem;animation-duration: 3s"></div></td>
          </tr>
        </tbody>
      </table>
      <style>
      body{
        background-image: url("` + logoCourse + `") !important;
        background-repeat: no-repeat !important;
        background-attachment: fixed !important;
        background-position: center bottom !important;
        background-size: 25% !important;
      }
      </style>
    </body>
</html>

  `;
  return afficheur;
}

open("http://localhost:1201");