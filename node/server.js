var http = require("http");

var server = http.createServer();
server.on("request", Requete);

var info = ["05:46", 2, ["00:00, 04:06"]];

function Requete(request, response){
    response.writeHead(200, {"Content-type": "application/json; charset=utf-8"});
    response.write(JSON.stringify(info));
    response.end();
}
server.listen(80);


// https://github.com/santigimeno/node-pcsclite
