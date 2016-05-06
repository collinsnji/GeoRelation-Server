
var io = require('socket.io-client');

function GeoRelationClient(address, cbGetGeoLocation, UserId) {

    var client = null;
    var SessToken = null;
    var TimeOut = null;

    var cbOnNearbyResult = null;
    var cbOnUpdateCompletion = null;

    var current_geolocation = null;

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

        client.on('update_complete', function() {
           if (typeof(cbOnUpdateCompletion) == "function") {
               cbOnUpdateCompletion(current_geolocation);
           }
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
            cbOnUpdateCompletion = next;
            client.emit('update_location', {
                                            geoLocation: geo,
                                            UserId: UserId,
                                            SessToken: SessToken
                                            });
        });
    };

    this.getNearby = function(callback) {
        client.emit('get_nearby', {
            UserId: UserId,
            SessToken: SessToken
            });
        cbOnNearbyResult = callback;
    };

}

function random(low, high) {
    return Math.random() * (high - low) + low;
}

//Test code

var Client = new GeoRelationClient("http://localhost:3000", function(next) {
    next({
        latitude: random(45.4991, 45.5057),
        longitude: random(73.0012, 74.000)
    });
}, "user");
Client.Connect();

setInterval(function() {

    Client.updateLocation(function(geo) {
        console.log("Current Coordinates: latitude " + geo.latitude + ", longitude " + geo.longitude)

        client.getNearby(function(result) {
            console.log(JSON.stringify(result));
        });

    });

}, 10000);

