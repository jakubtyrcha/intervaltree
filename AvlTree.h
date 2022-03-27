#include <optional>

using i32 = int;

struct AvlTree {
  using TKey = i32;
  using TValue = i32;

  struct Node;
  using NodePtr = Node*;
  static constexpr NodePtr kNullNode = nullptr;

  struct Node {
    NodePtr left = kNullNode;
    NodePtr right = kNullNode;
    i32 height = 0;
    TKey key;
    TValue value;
  };
  NodePtr root_{kNullNode};

  i32 max(i32 a, i32 b) { return (a > b) ? a : b; }

  // node allocation policy
  i32 nodesCount_ = 0;
  NodePtr NewNode();
  void DeleteNode(NodePtr);
  Node &NodeAt(NodePtr);
  Node const &NodeAt(NodePtr) const;
  i32 GetNodesCount() const;

  // private
  i32 GetBalance(NodePtr node) const;
  i32 GetHeight(NodePtr node) const;
  struct NodeWithParent {
    NodePtr node;
    NodePtr parent;
  };
  NodePtr GetLeftermost(NodePtr root);
  NodePtr SubtreeInsert(NodePtr, TKey k, TValue v);
  NodePtr SubtreeRemove(NodePtr, TKey k);
  NodePtr LeftRotate(NodePtr node);
  NodePtr RightRotate(NodePtr node);

  // operations
  i32 GetHeight() const;
  void Insert(TKey k, TValue v);
  void Remove(TKey k);
  std::optional<TValue> Find(TKey k) const;
};

AvlTree::NodePtr AvlTree::NewNode() {
  nodesCount_++;
  return new AvlTree::Node;
}

void AvlTree::DeleteNode(AvlTree::NodePtr ptr) {
  nodesCount_--;
  delete ptr;
}

AvlTree::Node &AvlTree::NodeAt(AvlTree::NodePtr ptr) {
  return *ptr;
}

AvlTree::Node const &AvlTree::NodeAt(AvlTree::NodePtr ptr) const {
  return *ptr;
}

i32 AvlTree::GetNodesCount() const {
  return nodesCount_;
}

i32 AvlTree::GetBalance(NodePtr node) const {
  return GetHeight(NodeAt(node).right) - GetHeight(NodeAt(node).left);
}

i32 AvlTree::GetHeight(NodePtr node) const {
  if (node == kNullNode) {
    return 0;
  }
  return NodeAt(node).height;
}

AvlTree::NodePtr AvlTree::GetLeftermost(NodePtr root) {
  NodePtr node = root;
  while (NodeAt(node).left) {
    node = NodeAt(node).left;
  }
  return node;
}

AvlTree::NodePtr AvlTree::LeftRotate(NodePtr x) {
  NodePtr y = NodeAt(x).right;
  NodePtr T2 = NodeAt(y).left;

  NodeAt(y).left = x;
  NodeAt(x).right = T2;

  NodeAt(x).height =
      max(GetHeight(NodeAt(x).left), GetHeight(NodeAt(x).right)) + 1;
  NodeAt(y).height =
      max(GetHeight(NodeAt(y).left), GetHeight(NodeAt(y).right)) + 1;

  return y;
}

AvlTree::NodePtr AvlTree::RightRotate(NodePtr y) {
  NodePtr x = NodeAt(y).left;
  NodePtr T2 = NodeAt(x).right;

  NodeAt(x).right = y;
  NodeAt(y).left = T2;

  NodeAt(y).height =
      max(GetHeight(NodeAt(y).left), GetHeight(NodeAt(y).right)) + 1;
  NodeAt(x).height =
      max(GetHeight(NodeAt(x).left), GetHeight(NodeAt(x).right)) + 1;

  return x;
}

AvlTree::NodePtr AvlTree::SubtreeInsert(NodePtr root, TKey k, TValue v) {
  if (root == kNullNode) {
    NodePtr node = NewNode();
    NodeAt(node).key = k;
    NodeAt(node).value = v;
    NodeAt(node).height = 1;
    return node;
  }

  if (k < NodeAt(root).key) {
    NodeAt(root).left = SubtreeInsert(NodeAt(root).left, k, v);
  } else if (NodeAt(root).key < k) {
    NodeAt(root).right = SubtreeInsert(NodeAt(root).right, k, v);
  } else {
    return root;
  }

  NodePtr left = NodeAt(root).left;
  NodePtr right = NodeAt(root).right;
  NodeAt(root).height = 1 + max(GetHeight(left), GetHeight(right));

  i32 balance = GetBalance(root);

  // left left
  if (balance < -1 && k < NodeAt(NodeAt(root).left).key) {
    return RightRotate(root);
  }

  // right right
  if (1 < balance && NodeAt(NodeAt(root).right).key < k) {
    return LeftRotate(root);
  }

  // left right
  if (balance < -1 && NodeAt(NodeAt(root).left).key < k) {
    NodeAt(root).left = LeftRotate(NodeAt(root).left);
    return RightRotate(root);
  }

  // right left
  if (1 < balance && k < NodeAt(NodeAt(root).right).key) {
    NodeAt(root).right = RightRotate(NodeAt(root).right);
    return LeftRotate(root);
  }

  return root;
}

AvlTree::NodePtr AvlTree::SubtreeRemove(NodePtr root, TKey k) {
  if (root == kNullNode) {
    return root;
  }
  NodePtr newRoot = root;
  if (k < NodeAt(root).key) {
    NodeAt(root).left = SubtreeRemove(NodeAt(root).left, k);
  } else if (NodeAt(root).key < k) {
    NodeAt(root).right = SubtreeRemove(NodeAt(root).right, k);
  } else {
    if (NodeAt(root).left == kNullNode) {
      newRoot = NodeAt(root).right;
    } else if (NodeAt(root).right == kNullNode) {
      newRoot = NodeAt(root).left;
    } else {
      NodePtr leftermost = GetLeftermost(NodeAt(root).right);
      newRoot = NewNode();
      NodeAt(newRoot).key = NodeAt(leftermost).key;
      NodeAt(newRoot).value = NodeAt(leftermost).value;
      NodeAt(newRoot).left = NodeAt(root).left;
      NodeAt(newRoot).right = SubtreeRemove(NodeAt(root).right, NodeAt(leftermost).key);
    }
    DeleteNode(root);
  }
  if (newRoot != kNullNode) {
    NodeAt(newRoot).height = 1 + max(GetHeight(NodeAt(newRoot).left),
                                     GetHeight(NodeAt(newRoot).right));
    i32 balance = GetBalance(newRoot);
    
    // left left
    if (balance < -1 && GetBalance(NodeAt(newRoot).left) <= 0) {
      return RightRotate(newRoot);
    }

    // right right
    if (1 < balance && 0 <= GetBalance(NodeAt(newRoot).right)) {
      return LeftRotate(newRoot);
    }

    // left right
    if(balance < -1 && 0 < GetBalance(NodeAt(newRoot).left)) {
      NodeAt(newRoot).left = LeftRotate(NodeAt(newRoot).left);
      return RightRotate(newRoot);
    }

    // right left
    if (1 < balance && GetBalance(NodeAt(newRoot).right) < 0) {
      NodeAt(newRoot).right = RightRotate(NodeAt(newRoot).right);
      return LeftRotate(newRoot);
    }
  }

  return newRoot;
}

i32 AvlTree::GetHeight() const { return GetHeight(root_); }

void AvlTree::Insert(TKey k, TValue v) { root_ = SubtreeInsert(root_, k, v); }

void AvlTree::Remove(TKey k) { root_ = SubtreeRemove(root_, k); }

std::optional<AvlTree::TValue> AvlTree::Find(TKey k) const {
  NodePtr currentNode = root_;
  while (currentNode != kNullNode) {
    if (k < NodeAt(currentNode).key) {
      currentNode = NodeAt(currentNode).left;
    } else if (NodeAt(currentNode).key < k) {
      currentNode = NodeAt(currentNode).right;
    } else {
      return NodeAt(currentNode).value;
    }
  }
  return std::nullopt;
}