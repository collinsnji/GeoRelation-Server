/**
 * Created by Gabriel Alacchi on 22/02/2016.
 */





function SearchTree(ComparisonFunc) {
    function TreeNode(val) {
        this.value = val;
        this.leftChild = null;
        this.rightChild = null;
        this.parent = null;

        this.isRoot = function () {
            return parent == null;
        }
        this.getSibling = function () {
            if (parent == null) return null;

            if (parent.leftChild == this)
                return parent.rightChild;
            else
                return parent.leftChild;

        }
    }

    this.Insert = function(value) {

    }


}