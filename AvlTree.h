#include <optional>

using i32 = int;

struct AvlTree {
  using TKey = i32;
  using TValue = i32;

  using NodePtr = void *;
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
  NodePtr NewNode();
  void DeleteNode(NodePtr);
  Node &NodeAt(NodePtr);
  Node const &NodeAt(NodePtr) const;

  // private
  i32 GetHeight(NodePtr node) const;
  NodePtr SubtreeInsert(NodePtr, TKey k, TValue v);
  NodePtr LeftRotate(NodePtr node);
  NodePtr RightRotate(NodePtr node);

  // operations
  i32 GetHeight() const;
  void Insert(TKey k, TValue v);
  void Remove(TKey k);
  std::optional<TValue> Find(TKey k) const;
};

AvlTree::NodePtr AvlTree::NewNode() { return new AvlTree::Node; }

void AvlTree::DeleteNode(AvlTree::NodePtr ptr) { delete ptr; }

AvlTree::Node &AvlTree::NodeAt(AvlTree::NodePtr ptr) {
  return *reinterpret_cast<AvlTree::Node *>(ptr);
}

AvlTree::Node const &AvlTree::NodeAt(AvlTree::NodePtr ptr) const {
  return *reinterpret_cast<AvlTree::Node *const>(ptr);
}

i32 AvlTree::GetHeight(NodePtr node) const {
  if (node == kNullNode) {
    return 0;
  }
  return NodeAt(node).height;
}

AvlTree::NodePtr AvlTree::LeftRotate(NodePtr x) {
    NodePtr y = NodeAt(x).right;
    NodePtr T2 = NodeAt(y).left;

    NodeAt(y).left = x;
    NodeAt(x).right = T2;

    NodeAt(x).height = max(GetHeight(NodeAt(x).left),   
                    GetHeight(NodeAt(x).right)) + 1;
    NodeAt(y).height = max(GetHeight(NodeAt(y).left),
                    GetHeight(NodeAt(y).right)) + 1;
 
    return y;
}

AvlTree::NodePtr AvlTree::RightRotate(NodePtr y) {
    NodePtr x = NodeAt(y).left;
    NodePtr T2 = NodeAt(x).right;
 
    NodeAt(x).right = y;
    NodeAt(y).left = T2;
 
    NodeAt(y).height = max(GetHeight(NodeAt(y).left),
                    GetHeight(NodeAt(y).right)) + 1;
    NodeAt(x).height = max(GetHeight(NodeAt(x).left),
                    GetHeight(NodeAt(x).right)) + 1;
 
    return x;
}

AvlTree::NodePtr AvlTree::SubtreeInsert(NodePtr currentNode, TKey k, TValue v) {
  if (currentNode == kNullNode) {
    NodePtr node = NewNode();
    NodeAt(node).key = k;
    NodeAt(node).value = v;
    return node;
  }

  if (k < NodeAt(currentNode).key) {
    NodeAt(currentNode).left = SubtreeInsert(NodeAt(currentNode).left, k, v);
  } else if (NodeAt(currentNode).key < k) {
    NodeAt(currentNode).right = SubtreeInsert(NodeAt(currentNode).right, k, v);
  } else {
      return currentNode;
  }

  NodePtr left = NodeAt(currentNode).left;
  NodePtr right = NodeAt(currentNode).right;
  NodeAt(currentNode).height = 1 + max(GetHeight(left), GetHeight(right));

  i32 balance = GetHeight(right) - GetHeight(left);

  // left left
  if (balance < -1 && k < NodeAt(NodeAt(currentNode).left).key) {
    return RightRotate(currentNode);
  }

  // right right
  if (1 < balance && NodeAt(NodeAt(currentNode).right).key < k) {
    return LeftRotate(currentNode);
  }

  // left right
  if (balance < -1 && NodeAt(NodeAt(currentNode).left).key < k) {
    NodeAt(currentNode).left = LeftRotate(NodeAt(currentNode).left);
    return RightRotate(currentNode);
  }

  // right left
  if (1 < balance && k < NodeAt(NodeAt(currentNode).right).key) {
    NodeAt(currentNode).right = RightRotate(NodeAt(currentNode).right);
    return LeftRotate(currentNode);
  }

  return currentNode;
}

i32 AvlTree::GetHeight() const {
    return GetHeight(root_);
}

void AvlTree::Insert(TKey k, TValue v) {
  root_ = SubtreeInsert(root_, k, v);
}

void AvlTree::Remove(TKey k) {}

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