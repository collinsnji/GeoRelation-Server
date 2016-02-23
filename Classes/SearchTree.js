/**
 * Created by Gabriel Alacchi on 22/02/2016.
 */





function SearchTree(LessThanOrEqual, Equal) {

    var root = null;

    function TreeNode(key, value) {
        this.key = key;
        this.value = value;
        this.leftChild = null;
        this.rightChild = null;
        this.parent = null;
    }

    TreeNode.prototype.isRoot = function () {
        return parent == null;
    };

    TreeNode.prototype.getSibling = function () {
        if (parent == null) return null;

        if (parent.leftChild === this)
            return parent.rightChild;
        else
            return parent.leftChild;

    }

}

SearchTree.prototype.Search = function(key) {
    if (root == null) return null;

    //search the tree iteratively
    var position = root;
    while (position != null) {
        if (Equal(key, position.key))
            return position.value;
        else if (LessThanOrEqual(key, position.key)) {
            position = position.leftChild;
        } else {
            position = position.rightChild;
        }
    }
    return null;

};

SearchTree.prototype.Insert = function(key, value) {
    var newNode = new TreeNode(key, value);
    if (root == null) {
        root = newNode;
        return newNode;
    }

    var position = root;
    while (position != null) {

        var condition = LessThanOrEqual(key, position.key);

        if (condition && position.leftChild == null) {
            position.leftChild = newNode;
            return newNode;
        } else if (!condition && position.rightChild == null) {
            position.rightChild == newNode;
            return newNode;
        }

        position = condition? position.leftChild : position.rightChild;

    }

    return null;

};