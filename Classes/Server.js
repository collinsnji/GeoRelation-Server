/**
 * Created by Gabriel Alacchi on 05/03/2016.
 */

var ProximityTree = require('../C++ Addons/ProximityTree/Debug/ProximityTree');
var ioModule = require('socket.io');
var SHA256 = require('crypto-js/sha256');

/*

 NODE_SET_PROTOTYPE_METHOD(tpl, "Insert", Insert);
 NODE_SET_PROTOTYPE_METHOD(tpl, "Remove", Remove);
 NODE_SET_PROTOTYPE_METHOD(tpl, "PrintOut", PrintOut);
 NODE_SET_PROTOTYPE_METHOD(tpl, "GetRoot", GetRoot);
 NODE_SET_PROTOTYPE_METHOD(tpl, "GetNearby", GetNearby);
 NODE_SET_PROTOTYPE_METHOD(tpl, "updateLocation", updateLocation);

 */

//contract_data is any data the client actually needs to store to manage users in their applciation
function ProximityUser(UserId, initialLocation, sessToken, _data) {
    this.UserId = UserId;
    this.treeIndex = -1;
    this.geoLocation = initialLocation;
    this.sessToken = sessToken;
    this.data = _data;

    var timeoutid = 0;

    this.setUserTimeout = function(ProximityApi, timeout) {

        if (timeoutid != 0) {
            clearTimeout(timeoutid);
        }
        timeoutid = setTimeout(function () {
            console.log("User session, UserID: {0}, SessToken: {1} just timed out!", UserId, sessToken);
            ProximityApi.RemoveUser(this);
        }, timeout);

    }

}

//Constructs object that interfaces with the c++ data structure ProximityTree
function ProximityAPI(distanceBenchmark, cbGetGeoLocation, capacity) {
//Requires a distance bench mark and a callback that allows the server to get geolocation

    var users = {};
    var uid_to_tree = {};

    this.geoLocation = cbGetGeoLocation();

    var tree = new ProximityTree.ProximityTree(capacity, this.geoLocation.latitude, this.geoLocation.longitude);

    this.RegisterUser = function(User) {
        User.treeIndex = tree.Insert(User.geoLocation.latitude, User.geoLocation.longitude);
        users[User.treeIndex] = User;
        uid_to_tree[User.UserId] = User.treeIndex;
    };

    this.UpdateUser = function(User) {
        var newTreeIndex = tree.updateLocation(User.treeIndex, User.geoLocation.latitude, User.geoLocation.longitude);
        if (newTreeIndex != User.treeIndex) {

            users.remove(User.treeIndex);
            uid_to_tree[User.UserId] = newTreeIndex;

            User.treeIndex = newTreeIndex;
            users.add(newTreeIndex, User);

        }
    };

    this.GetNearbyUsers = function(User) {
        var results = tree.GetNearby(User.treeIndex, distanceBenchmark);

        var user_array = [];
        for (var res in results) {
            var User_ref = users[res.Node];
            var item = {
                Distance: res.Distance,
                User: User_ref
            };
            user_array.add(item);
        }

        return user_array;

    };

    this.IsUserRegistered = function(UserId) {
        return (UserId in uid_to_tree);
    };

    this.RemoveUser = function(User) {
        tree.Remove(User.treeIndex);
        users.remove(User.treeIndex);
        uid_to_tree.remove(User.UserId);
    };

}

function generateSessionToken(UserId) {

    var token = SHA256(UserId);

    var possible = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";

    for( var i=0; i < 5; i++ )
        token += possible.charAt(Math.floor(Math.random() * possible.length));

    return token;

}

function geoRelationServer(app, distanceBenchmark, cbGetGeoLocation, capacity) {

    var api = ProximityAPI(distanceBenchmark, cbGetGeoLocation, capacity);
    var sockio = new ioModule(app, {serveClient: true});

    //Create a session token for a user and add a timeout for this user
    sockio.on('create_sess', function(data) {
        if (data.UserId == undefined || api.IsUserRegistered(data.UserId)) {
            //The user is already registered, ignore his attempt to recreate the session
            return;
        }

        var token = generateSessionToken(data.UserId);

        var User = ProximityUser(data.userId, data.geoLocation, token, data._data);

        User.setUserTimeout(api, 600000);



    });

}


//Export proximity User
module.exports.ProximityUser = ProximityUser;
module.exports.GeoRelationServer = geoRelationServer;