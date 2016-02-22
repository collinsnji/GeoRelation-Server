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

        this.isRoot = function () {
            return parent == null;
        }
        this.getSibling = function () {
            if (parent == null) return null;

            if (parent.leftChild === this)
                return parent.rightChild;
            else
                return parent.leftChild;

        }
    }

    this.Search = function(key) {
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

    }

    this.Insert = function(key, value) {
        var newNode = new TreeNode(key, value);
        if (root == null)
            root = newNode;

        var position = root;
        while (position != null) {
            if (Equal(key, position.key))
                SpliceInsertion(position, newNode, true);

            var comparison = LessThanOrEqual(position.key, key);
            var placement = comparison? position.leftChild : position.rightChild;

            

            if (LessThanOrEqual(position.key, key)) {
                if (position.leftChild == null)
                    SpliceInsertion(position, newNode, true)
                else if (!LessThanOrEqual(position.leftChild))
            }
        }

    }

    function SpliceInsertion(insertPos, newNode, left) {

        if (left && insertPos.leftChild == null)
            insertPos.leftChild = newNode;
        else if (left && insertPos.leftChild != null) {
            if (LessThanOrEqual(insertPos.leftChild.key, newNode.key)) {
                newNode.leftChild = insertPos.leftChild;
                insertPos.leftChild = newNode;
            } else {
                newNode.rightChild = insertPos.rightChild;
                insertPos.leftChild = newNode;
            }
        }
        else if (!left && insertPos.rightChild == null) {
            insertPos.rightChild = newNode;
        }
        else if (!left && insertPos.rightChild != null) {
            if (LessThanOrEqual(insertPos.rightChild.key, newNode.key)) {
                newNode.leftChild = insertPos.rightChild;
                insertPos.rightChild = newNode;
            } else {
                newNode.rightChild = insertPos.rightChild;
                insertPos.rightChild = newNode;
            }
        }

        newNode.parent = insertPos;
        if (newNode.leftChild != null)
            newNode.leftChild.parent = newNode;
        if (newNode.rightChild != null)
            newNode.rightChild.parent = newNode;

    }

}