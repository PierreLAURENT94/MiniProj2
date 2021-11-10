# Course Kronos
![](/Inkscape/readmesf.png)
## Qu'est-ce que **Course Kronos** ?
**Course Kronos** permet de **chronométrer** chaque coureur d'une **course** (Tour de France, Marathon, Triathlon, Biathlon...) avec prise en charge des éventuelles **étapes**, tout ça avec via des **cartes PCSC**.
## Pour node faire ceci :
```bat
npm init
npm install electron pcsclite smartcard open
npm i -D electron-rebuild

// Supprimez le dossier node-modules, ainsi que le fichier packages-lock.json

npm i
.\node_modules\.bin\electron-rebuild.cmd

//Ajouter:
//"scripts": {
//    "start": "electron ."
//},
//dans le le fichier packages.json

npm start
```
> Pierre LAURENT • Vincent LORCY • Nov 2021
