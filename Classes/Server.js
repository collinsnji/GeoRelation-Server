/**
 * Created by Gabriel Alacchi on 05/03/2016.
 */

var ProximityTree = require('../C++ Addons/ProximityTree/Debug/ProximityTree');

/*

 NODE_SET_PROTOTYPE_METHOD(tpl, "Insert", Insert);
 NODE_SET_PROTOTYPE_METHOD(tpl, "Remove", Remove);
 NODE_SET_PROTOTYPE_METHOD(tpl, "PrintOut", PrintOut);
 NODE_SET_PROTOTYPE_METHOD(tpl, "GetRoot", GetRoot);
 NODE_SET_PROTOTYPE_METHOD(tpl, "GetNearby", GetNearby);
 NODE_SET_PROTOTYPE_METHOD(tpl, "updateLocation", updateLocation);

 */

//contract_data is any data the client actually needs to store to manage users in their applciation
function ProximityUser(ID, initialLocation, _data) {
    this.ID = ID;
    this.treeIndex = -1;
    this.geoLocation = initialLocation;
    this.data = _data;
}

//Constructs object that interfaces with the c++ data structure ProximityTree
function ProximityAPI(distanceBenchmark, cbGetGeoLocation, capacity) {
//Requires a distance bench mark and a callback that allows the server to get geolocation

    var users = {};

    this.geoLocation = cbGetGeoLocation();

    var tree = new ProximityTree.ProximityTree(capacity, this.geoLocation.latitude, this.geoLocation.longitude);

    this.RegisterUser = function(User) {
        User.treeIndex = tree.Insert(User.geoLocation.latitude, User.geoLocation.longitude);
        users[User.treeIndex] = User;
    }

}

//Export proximity User
module.exports.ProximityUser = ProximityUser;