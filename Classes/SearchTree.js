/**
 * Created by Gabriel Alacchi on 22/02/2016.
 */





function SearchTree(LessThanOrEqual, Equal) {

    this.root = null;

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
    if (this.root == null) return null;

    //search the tree iteratively
    var position = this.root;
    while (position != null) {
        if (Equal(key, position.key))
            return position;
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
    if (this.root == null) {
        this.root = newNode;
        return newNode;
    }

    var position = this.root;
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

//Severs the node from it's parent
function sever(TreeNode) {
    if (TreeNode.parent.leftChild == TreeNode)
        TreeNode.parent.leftChild = null;
    else
        TreeNode.parent.rightChild = null;
    return TreeNode.parent;
}

SearchTree.prototype.Remove = function(TreeNode) {


    //CASE 1: The node has no children
    if (TreeNode.leftChild == null && TreeNode.rightChild == null) {
        if (TreeNode == this.root) {
            delete this.root;
        } else {
            sever(TreeNode);
        }
    }

    //CASE 2: The node has only a left sided child
    if (TreeNode.leftChild != null && TreeNode.rightChild == null) {
        sever(TreeNode).leftChild =  TreeNode.leftChild;
    }

    if (TreeNode.leftChild == null && TreeNode.rightChild != null) {
        sever(TreeNode).rightChild = TreeNode.rightChild;
    }

    //CASE 3: The node has 2 children recursion is necessary
    if (TreeNode.leftChild != null && TreeNode.rightChild != null) {
        //Take it's in-order child (right)
        if (TreeNode == this.root) {
            //special case
            this.root = TreeNode.rightChild;

        } else {
            var parent = TreeNode.parent;
            TreeNode.parent = null;
            if (parent.leftChild == TreeNode)
                parent.leftChild = TreeNode.rightChild;
            else
                parent.rightChild = TreeNode.rightChild;
            //TODO: rewrite in C++ and physically harm myself afterwards
        }
    }

};