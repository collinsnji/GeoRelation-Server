
var io = require('socket.io-client');

function GeoRelationClient(address, cbGetGeoLocation, UserId) {

    var client = null;
    var SessToken = null;
    var TimeOut = null;

    var cbOnNearbyResult = null;

    this.Connect = function() {

        client = io(address);

        client.on('connect', function() {
           console.log("connected to server!");
        });

        client.on('error', function(err) {
            console.log("Error from server: " + err);
        });

        client.on('session_init', function(data) {
            SessToken = data.SessToken;
            TimeOut = data.TimeOut;
            console.log("Session initialized");
        });

        client.on('nearby_response', function(data) {
            if (cbOnNearbyResult != null && typeof(cbOnNearbyResult) == "function")
                cbOnNearbyResult(data);
        });

        cbGetGeoLocation(function(geoLocation) {
            client.emit('create_sess',
                {
                    UserId: UserId,
                    geoLocation: geoLocation
                });
        });

    };

    this.updateLocation = function(next) {
        cbGetGeoLocation(function(geo) {
            client.emit('update_location', {
                                            geoLocation: geo,
                                            UserId: UserId,
                                            SessToken: SessToken
                                            });
            if (typeof(next) == "function") {
                next(geo);
            }
        });
    };

    this.getNearby = function(next) {
        client.emit('get_nearby', {
            UserId: UserId,
            SessToken: SessToken
            });
        cbOnNearbyResult = next;
    };

}



//Test code

var Client = new GeoRelationClient("http://localhost:3000", function(next) {
    next({
        latitude: 10.0,
        longitude: 10.02
    });
}, "user");
Client.Connect();

