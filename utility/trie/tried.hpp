#ifndef TIRE_H
#define TIRE_H
#include <functional>
#include <map>
#include <memory>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>
#include <mutex>
#include <shared_mutex>
#include <cassert>

namespace WebServer::TRIE
{

class TrieNode
{
  public:
    explicit TrieNode(const char &value);
    TrieNode(TrieNode &&other) noexcept;
    virtual ~TrieNode() = default;
    auto HasChild(char key_char) -> bool;
    auto HasChildren() -> bool;
    auto IsEndNode() -> bool;
    auto GetKeyChar() -> char;
    auto InsertChildNode(char key_char, std::unique_ptr<TrieNode> &&child) -> std::unique_ptr<TrieNode> *;
    auto GetChildNode(char key_char) -> std::unique_ptr<TrieNode> *;
    void RemoveChildNode(char key_char);
    void SetEndNode(bool is_end);

  private:
    std::map<char, std::unique_ptr<TrieNode>> children_;

  protected:
    char key_value_;
    bool is_end_;
};

/**
 * TrieNodeWithValue is a node that marks the ending of a key, and it can
 * hold a value of any type T.
 */
template <class T> class TrieNodeWithValue : public TrieNode
{
  public:
    [[maybe_unused]] explicit TrieNodeWithValue(char key_char, T value);
    explicit TrieNodeWithValue(TrieNode &&trieNode, T value);
    auto GetValue() -> T &;
    ~TrieNodeWithValue() override = default;

  private:
    T value_;
};
template <class T> auto TrieNodeWithValue<T>::GetValue() -> T &
{
    return value_;
}
template <class T> TrieNodeWithValue<T>::TrieNodeWithValue(TrieNode &&trieNode, T value) : TrieNode(std::move(trieNode))
{
    value_ = value;
    is_end_ = true;
}
template <class T> [[maybe_unused]] TrieNodeWithValue<T>::TrieNodeWithValue(char key_char, T value) : TrieNode(key_char)
{
    value_ = value;
    is_end_ = true;
}

class Trie
{
  public:
    Trie();
    ~Trie() = default;
    template <class T> auto Insert(const std::string &key, const T &value) -> bool;
    auto Remove(const std::string &key) -> bool;
    template <class T> auto GetValue(const std::string &key, bool *success) -> T;

  private:
    std::unique_ptr<TrieNode> root_;
    std::shared_mutex mul_;
};

template <class T> auto Trie::GetValue(const std::string &key, bool *success) -> T
{
    std::shared_lock<std::shared_mutex> lock(mul_);
    std::unique_ptr<TrieNode> *temp = &root_;
    for (auto &c : key)
    {
        if (!(*temp)->HasChild(c))
        {
            *success = false;
            return T();
        }
        temp = (*temp)->GetChildNode(c);
    }
    if ((*temp)->IsEndNode())
    {
        *success = true;
        return dynamic_cast<TrieNodeWithValue<T> *>(temp->get())->GetValue();
    }
    (*success) = false;
    return T();
}

template <class T> auto Trie::Insert(const std::string &key, const T &value) -> bool
{
    std::shared_lock<std::shared_mutex> lock(mul_);
    if (key.empty())
    {
        return false;
    }
    std::unique_ptr<TrieNode> *temp = &root_;
    for (auto &c : key)
    {
        if (!(*temp)->HasChild(c))
        {
            std::unique_ptr<TrieNode> tem(new TrieNode(c));
            (*temp)->InsertChildNode(c, std::move(tem));
        }
        temp = (*temp)->GetChildNode(c);
    }
    if ((*temp)->IsEndNode())
    {
        return false;
    }
    (*temp).reset(new TrieNodeWithValue<T>(std::move(**temp), T(value)));
    return true;
}
inline TrieNode::TrieNode(const char &value)
{
    key_value_ = value;
    is_end_ = false;
}
inline TrieNode::TrieNode(TrieNode &&other) noexcept
{
    this->key_value_ = other.key_value_;
    this->is_end_ = other.is_end_;
    this->children_ = std::move(other.children_);
}
inline bool TrieNode::HasChild(char key_char)
{
    return  children_.find(key_char) != children_.end();
}
inline bool TrieNode::HasChildren()
{
    return !children_.empty();
}
inline bool TrieNode::IsEndNode()
{
    return is_end_;
}
inline char TrieNode::GetKeyChar()
{
    return key_value_;
}
inline std::unique_ptr<TrieNode> *TrieNode::InsertChildNode(char key_char, std::unique_ptr<TrieNode> &&child)
{
    if (this->HasChild(key_char))
    {
        return nullptr;
    }
    if (key_char != child->key_value_)
    {
        return nullptr;
    };
    children_.insert({key_char, std::move(child)});
    auto tem = &(children_[key_char]);
    return tem;
}
inline std::unique_ptr<TrieNode> *TrieNode::GetChildNode(char key_char)
{
    if (!this->HasChild(key_char))
    {
        return nullptr;
    }
    return &children_[key_char];
}
inline void TrieNode::RemoveChildNode(char key_char)
{
    assert(HasChild(key_char));
    children_.erase({key_char});
}

inline void TrieNode::SetEndNode(bool is_end)
{
    is_end_ = is_end;
}
inline Trie::Trie()
{
    std::unique_lock<std::shared_mutex> lock(mul_);
    std::unique_ptr<TrieNode> temp(new TrieNode('#'));
    root_ = std::move(temp);
}

inline bool Trie::Remove(const std::string &key)
{
    std::unique_lock<std::shared_mutex> lock(mul_);
    if (key.empty())
    {
        return false;
    }
    bool fl = false;
    bool st = true;
    std::function<void(std::unique_ptr<TrieNode> *, int)> dfs = [&](std::unique_ptr<TrieNode> *root, int x) {
        if (static_cast<int>(key.size()) == x)
        {
            if ((*root)->IsEndNode())
            {
                fl = true;
                (*root)->SetEndNode(false);
            }
            else
            {
                fl = false;
            }
            return;
        }
        if (!(*root)->HasChild(key[x]))
        {
            return;
        }
        dfs((*root)->GetChildNode(key[x]), x + 1);
        if (fl && !(*(*root)->GetChildNode(key[x]))->HasChildren())
        {

            if (st)
            {
                (*root)->RemoveChildNode(key[x]);
            }
            if ((*root)->IsEndNode())
            {
                st = false;
            }
        }
    };
    dfs(&(root_), 0);
    return fl;
}

} 

#endif