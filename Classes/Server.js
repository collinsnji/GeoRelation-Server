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
    this.SessToken = sessToken;
    this.data = _data;
    this.timeoutid = 0;
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

function geoRelationServer(app, distanceBenchmark, cbGetGeoLocation, capacity, userTimeOut) {

    var api = new ProximityAPI(distanceBenchmark, cbGetGeoLocation, capacity);
    var sockio = new ioModule(app, {serveClient: true});

    //Logging
    var verbose = false;

    //Callbacks
    var cbNewSession = null;

    var ValidTokens = {};
    var Users = {};

    var isValidatedUserData = function(data) {
        var id = data.UserId;
        var token = data.SessToken;

        var tokenExists = id != undefined && token != undefined && (id in ValidTokens);

        return (id != undefined && api.IsUserRegistered(id) && tokenExists && token === ValidTokens[id]);
    };

    var error = function(socket, message) {
        console.log(message);
        socket.emit('error', message);
    };

    var CustomEvents = [];

    var setUserTimeout = function(User, timeout) {

        var timeoutid = setTimeout(function() {
            api.RemoveUser(User);
            ValidTokens.remove(User.UserId);
            Users.remove(User.UserId);
        }, timeout);

        if (User.timeoutid != 0)
            clearTimeout(User.timeoutid);

        User.timeoutid = timeoutid;

    };

    //All event definitions go here
    sockio.on('connection', function(socket) {

        var address = socket.handshake.address;
        console.log("New Socket Connection: {0}:{1}", address.address, address.port);

        //Create a session token for a user and add a timeout for this user
        socket.on('create_sess', function(data) {
            if (data.UserId == undefined || api.IsUserRegistered(data.UserId)) {
                //The user is already registered, ignore his attempt to recreate the session
                socket.emit('error', 'A session already exists for this user');
                return;
            }

            var token = generateSessionToken(data.UserId);
            ValidTokens[data.UserId] = token;

            var User = new ProximityUser(data.UserId, data.geoLocation, token, data._data);

            setUserTimeout(api, userTimeOut);

            Users[data.UserId] = User;

            if (cbNewSession != null)
                cbNewSession(User);

            socket.emit('session_init', {
                SessToken: token,
                TimeOut: userTimeOut
            });

        });

        socket.on('update_location', function(data) {
            if (isValidatedUserData(data)) {
                Error(socket, "Failed update_location request, validation failed!");
                return;
            } else if (data.geoLocation == undefined) {
                //The user hasn't provided
                Error(socket, "No geoLocation data provided in 'update_location'");
                return;
            }

            if (verbose)
                console.log("update_location request {0}", JSON.stringify(data));
            else
                console.log("update location request");

            //The proper checks have been done to make sure this user is in this dictionary
            var User = Users[data.UserId];
            setUserTimeout(api, userTimeOut);
            User.geoLocation = data.geoLocation;

            api.UpdateUser(User);

        });

        socket.on('get_nearby', function(data) {
            if (isValidatedUserData(data)) {
                Error(socket, "Failed update_location request, validation failed!");
                return;
            }

            if (verbose)
                console.log("get_nearby request {0}", JSON.stringify(data));
            else
                console.log("get_nearby request");

            var User = Users[data.UserId];
            setUserTimeout(api, userTimeOut);
            var nearby = api.GetNearbyUsers(User);

            console.log("Found nearby users for {0}: \n{1}", User.UserId, JSON.stringify(nearby));

        });

        socket.on('disconnect', function(data) {
            if (isValidatedUserData(data)) {
                Error(socket, "Failed disconnect request, validation failed!");
                return;
            }

            var User = Users[data.UserId];
            api.RemoveUser(User);
            ValidTokens.remove(User.UserId);
            Users.remove(User.UserId);

            socket.close();

        });

        for (var event in CustomEvents) {
            socket.on(event.EventName, function(data) {
                if (isValidatedUserData(data)) {
                    Error(socket, "Failed update_location request, validation failed!");
                    return;
                }

                if (verbose)
                    console.log("{1} request {0}", JSON.stringify(data), event.EventName);
                else
                    console.log("{0} request", event.EventName);

                var User = Users[data.UserId];
                setUserTimeout(api, userTimeOut);
                event.Callback(data, User);
            });
        }

    });

    this.registerCustomEvent = function(EventName, cbOnEvent){
        CustomEvents.add({
            EventName: EventName,
            Callback: cbOnEvent
        });
    };
    this.setNewSessionCallback = function(cb) {
        cbNewSession = cb;
    };
    this.getVerbose = function() {
        return verbose;
    };
    this.setVerbose = function(val) {
        verbose = val;
    };

}


//Export proximity User
module.exports.ProximityUser = ProximityUser;
module.exports.GeoRelationServer = geoRelationServer;