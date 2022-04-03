#include <optional>
#include <vector>

using i32 = int;

template <typename TInterval, typename TValue> struct IntervalMultiTree {
  using TIntervalEdge = decltype(TInterval::end);

  struct Node;
  using NodePtr = Node *;
  static constexpr NodePtr kNullNode = nullptr;

  struct Node {
    NodePtr left = kNullNode;
    NodePtr right = kNullNode;
    i32 height = 0;
    TInterval nodeInterval;
    TIntervalEdge subtreeEnd;
    std::vector<TValue> values;
  };
  NodePtr root_{kNullNode};

  template <typename T> T max(T a, T b) { return (a > b) ? a : b; }

  // node allocation policy
  i32 nodesCount_ = 0;
  NodePtr NewNode();
  void DeleteNode(NodePtr);
  Node &NodeAt(NodePtr);
  Node const &NodeAt(NodePtr) const;
  i32 GetNodesCount() const;
  i32 GetHeight(NodePtr node) const {
    if (node == kNullNode) {
      return 0;
    }
    return NodeAt(node).height;
  }
  i32 GetBalance(NodePtr node) const {
    return GetHeight(NodeAt(node).right) - GetHeight(NodeAt(node).left);
  }
  NodePtr GetLeftermost(NodePtr root) const {
    NodePtr node = root;
    while (NodeAt(node).left) {
      node = NodeAt(node).left;
    }
    return node;
  }
  void AdjustSubtreeMax(NodePtr root) {
    TIntervalEdge subtreeEnd = NodeAt(root).nodeInterval.end;
    if (NodeAt(root).left != kNullNode) {
      subtreeEnd = max(subtreeEnd, NodeAt(NodeAt(root).left).subtreeEnd);
    }
    if (NodeAt(root).right != kNullNode) {
      subtreeEnd = max(subtreeEnd, NodeAt(NodeAt(root).right).subtreeEnd);
    }
    NodeAt(root).subtreeEnd = subtreeEnd;
  }

  NodePtr LeftRotate(NodePtr node);
  NodePtr RightRotate(NodePtr node);

  NodePtr SubtreeInsert(NodePtr, TInterval i, TValue v);
  NodePtr SubtreeRemove(NodePtr, TInterval i);
  std::optional<NodePtr> Find(TInterval i);
  void SubtreeCollectQueryValues(NodePtr, TIntervalEdge point,
                                 std::vector<TValue> &outAccumulator);

  // operations
  i32 GetHeight() const;
  void Insert(TInterval k, TValue v);
  void Remove(TInterval k);
  void CollectQueryValues(TIntervalEdge point,
                          std::vector<TValue> &outAccumulator);
};

template <typename TInterval, typename TValue>
IntervalMultiTree<TInterval, TValue>::NodePtr
IntervalMultiTree<TInterval, TValue>::NewNode() {
  nodesCount_++;
  return new Node;
}

template <typename TInterval, typename TValue>
void IntervalMultiTree<TInterval, TValue>::DeleteNode(NodePtr ptr) {
  nodesCount_--;
  delete ptr;
}

template <typename TInterval, typename TValue>
IntervalMultiTree<TInterval, TValue>::Node &
IntervalMultiTree<TInterval, TValue>::NodeAt(NodePtr ptr) {
  return *ptr;
}

template <typename TInterval, typename TValue>
IntervalMultiTree<TInterval, TValue>::Node const &
IntervalMultiTree<TInterval, TValue>::NodeAt(NodePtr ptr) const {
  return *ptr;
}

template <typename TInterval, typename TValue>
i32 IntervalMultiTree<TInterval, TValue>::GetNodesCount() const {
  return nodesCount_;
}

template <typename TInterval, typename TValue>
IntervalMultiTree<TInterval, TValue>::NodePtr
IntervalMultiTree<TInterval, TValue>::LeftRotate(NodePtr x) {
  NodePtr y = NodeAt(x).right;
  NodePtr T2 = NodeAt(y).left;

  NodeAt(y).left = x;
  NodeAt(x).right = T2;

  AdjustSubtreeMax(x);
  AdjustSubtreeMax(y);

  NodeAt(x).height =
      max(GetHeight(NodeAt(x).left), GetHeight(NodeAt(x).right)) + 1;
  NodeAt(y).height =
      max(GetHeight(NodeAt(y).left), GetHeight(NodeAt(y).right)) + 1;

  return y;
}

template <typename TInterval, typename TValue>
IntervalMultiTree<TInterval, TValue>::NodePtr
IntervalMultiTree<TInterval, TValue>::RightRotate(NodePtr y) {
  NodePtr x = NodeAt(y).left;
  NodePtr T2 = NodeAt(x).right;

  NodeAt(x).right = y;
  NodeAt(y).left = T2;

  AdjustSubtreeMax(y);
  AdjustSubtreeMax(x);

  NodeAt(y).height =
      max(GetHeight(NodeAt(y).left), GetHeight(NodeAt(y).right)) + 1;
  NodeAt(x).height =
      max(GetHeight(NodeAt(x).left), GetHeight(NodeAt(x).right)) + 1;

  return x;
}

template <typename TInterval, typename TValue>
IntervalMultiTree<TInterval, TValue>::NodePtr
IntervalMultiTree<TInterval, TValue>::SubtreeInsert(NodePtr root, TInterval i,
                                                    TValue v) {
  if (root == kNullNode) {
    NodePtr node = NewNode();
    NodeAt(node).nodeInterval = i;
    NodeAt(node).subtreeEnd = i.end;
    NodeAt(node).height = 1;
    NodeAt(node).values.emplace_back(v);
    return node;
  }

  if (i.begin < NodeAt(root).nodeInterval.begin) {
    NodeAt(root).left = SubtreeInsert(NodeAt(root).left, i, v);
  } else if (NodeAt(root).nodeInterval.begin < i.begin) {
    NodeAt(root).right = SubtreeInsert(NodeAt(root).right, i, v);
  } else {
    // modify interval and subtree
    NodeAt(root).values.emplace_back(v);
    return root;
  }

  NodePtr left = NodeAt(root).left;
  NodePtr right = NodeAt(root).right;
  NodeAt(root).height = 1 + max(GetHeight(left), GetHeight(right));

  AdjustSubtreeMax(root);

  i32 balance = GetBalance(root);

  // left left
  if (balance < -1 && i.begin < NodeAt(NodeAt(root).left).nodeInterval.begin) {
    return RightRotate(root);
  }

  // right right
  if (1 < balance && NodeAt(NodeAt(root).right).nodeInterval.begin < i.begin) {
    return LeftRotate(root);
  }

  // left right
  if (balance < -1 && NodeAt(NodeAt(root).left).nodeInterval.begin < i.begin) {
    NodeAt(root).left = LeftRotate(NodeAt(root).left);
    return RightRotate(root);
  }

  // right left
  if (1 < balance && i.begin < NodeAt(NodeAt(root).right).nodeInterval.begin) {
    NodeAt(root).right = RightRotate(NodeAt(root).right);
    return LeftRotate(root);
  }

  return root;
}

template <typename TInterval, typename TValue>
IntervalMultiTree<TInterval, TValue>::NodePtr
IntervalMultiTree<TInterval, TValue>::SubtreeRemove(NodePtr root, TInterval i) {
  if (root == kNullNode) {
    return root;
  }
  NodePtr newRoot = root;
  if (i.begin < NodeAt(root).nodeInterval.begin) {
    NodeAt(root).left = SubtreeRemove(NodeAt(root).left, i);
  } else if (NodeAt(root).nodeInterval.begin < i.begin) {
    NodeAt(root).right = SubtreeRemove(NodeAt(root).right, i);
  } else {
    if (NodeAt(root).left == kNullNode) {
      newRoot = NodeAt(root).right;
    } else if (NodeAt(root).right == kNullNode) {
      newRoot = NodeAt(root).left;
    } else {
      NodePtr leftermost = GetLeftermost(NodeAt(root).right);
      newRoot = NewNode();
      NodeAt(newRoot).nodeInterval = NodeAt(leftermost).nodeInterval;
      NodeAt(newRoot).values = NodeAt(leftermost).values; // TODO: move
      NodeAt(newRoot).left = NodeAt(root).left;
      NodeAt(newRoot).right =
          SubtreeRemove(NodeAt(root).right, NodeAt(leftermost).nodeInterval);
    }
    DeleteNode(root);
  }
  if (newRoot != kNullNode) {
    NodeAt(newRoot).height = 1 + max(GetHeight(NodeAt(newRoot).left),
                                     GetHeight(NodeAt(newRoot).right));
    i32 balance = GetBalance(newRoot);
    AdjustSubtreeMax(newRoot);

    // left left
    if (balance < -1 && GetBalance(NodeAt(newRoot).left) <= 0) {
      return RightRotate(newRoot);
    }

    // right right
    if (1 < balance && 0 <= GetBalance(NodeAt(newRoot).right)) {
      return LeftRotate(newRoot);
    }

    // left right
    if (balance < -1 && 0 < GetBalance(NodeAt(newRoot).left)) {
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

template <typename TInterval, typename TValue>
std::optional<typename IntervalMultiTree<TInterval, TValue>::NodePtr>
IntervalMultiTree<TInterval, TValue>::Find(TInterval i) {
  NodePtr currentNode = root_;
  while (currentNode != kNullNode) {
    if (i.begin < NodeAt(currentNode).nodeInterval.begin) {
      currentNode = NodeAt(currentNode).left;
    } else if (NodeAt(currentNode).nodeInterval.begin < i.end) {
      currentNode = NodeAt(currentNode).right;
    } else {
      return NodeAt(currentNode).value;
    }
  }
  return std::nullopt;
}

template <typename TInterval, typename TValue>
void IntervalMultiTree<TInterval, TValue>::Insert(TInterval i, TValue v) {
  root_ = SubtreeInsert(root_, i, v);
}

template <typename TInterval, typename TValue>
void IntervalMultiTree<TInterval, TValue>::Remove(TInterval i) {
  root_ = SubtreeRemove(root_, i);
}

template <typename TInterval, typename TValue>
void IntervalMultiTree<TInterval, TValue>::SubtreeCollectQueryValues(
    NodePtr root, TIntervalEdge point, std::vector<TValue> &outAccumulator) {
  if (root == kNullNode) {
    return;
  }

  if (NodeAt(root).nodeInterval.Contains(point)) {
    for (auto &v : NodeAt(root).values) {
      outAccumulator.push_back(v);
    }
  }

  if (point < NodeAt(root).subtreeEnd) {
    SubtreeCollectQueryValues(NodeAt(root).left, point, outAccumulator);
  }

  if (NodeAt(root).nodeInterval.begin < point &&
      point < NodeAt(root).subtreeEnd) {
    SubtreeCollectQueryValues(NodeAt(root).right, point, outAccumulator);
  }
}

template <typename TInterval, typename TValue>
void IntervalMultiTree<TInterval, TValue>::CollectQueryValues(
    TIntervalEdge point, std::vector<TValue> &outAccumulator) {
  SubtreeCollectQueryValues(root_, point, outAccumulator);
}