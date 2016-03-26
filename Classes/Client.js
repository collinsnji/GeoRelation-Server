
var io = require('socket.io-client');

function GeoRelationClient(address, cbGetGeoLocation, UserId) {

    var client = null;
    var SessToken = null;
    var TimeOut = null;

    var cbOnNearbyResult = null;

    this.setNearbyResultCallback = function(callback) {
        cbOnNearbyResult = callback;
    };

    this.Connect = function() {
        var geoLocation = cbGetGeoLocation();

        client = io(address);

        client.on('error', function(err) {
            console.log("Error from server: " + err);
        });

        client.on('session_init', function(data) {
            SessToken = data.SessToken;
            TimeOut = data.TimeOut;
        });

        client.on('nearby_response', function(data) {
            if (cbOnNearbyResult != null)
                cbOnNearbyResult(data);
        });

        client.emit('create_sess',
        {
            UserId: UserId,
            geoLocation: geoLocation
        });

    }



}