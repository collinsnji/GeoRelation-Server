
var express = require('express');
var http = require('http');
var app = express();
var fs = require('fs');

http.createServer(app).listen(3000, function() {
    console.log("Started listening on port 3000");

    var GeoServerModule = require('./Classes/Server.js');

    var Server = new GeoServerModule.GeoRelationServer(this, 100.0, function(cbResult) {
        cbResult({
            latitude: 45.5017,
            longitude: 73.5673
        });
    }, 10000, 60000*10);

    fs.readFile('./Example/index.html', function(err, html) {

        if (err) {
            throw err;
        }
        app.get('/', function(req, res) {
            res.writeHeader(200, {"Content-Type": "text/html"});
            res.write(html);
            res.end();
        });

    });

    console.log("Server is initialized...");

});

