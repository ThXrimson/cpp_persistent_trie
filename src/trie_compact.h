#ifndef TRIE_H
#define TRIE_H
#include <format>
#include <fstream>
#include <string>
#include <unordered_map>
#include <vector>

// Trie节点定义
struct TrieNode {
    bool isWord = false;
    std::unordered_map<char16_t, size_t> children;
};

class Trie {
public:
    Trie(): nodes{TrieNode()} {
    }

    explicit Trie(std::vector<TrieNode> &&nodes): nodes(move(nodes)) {
    }

    ~Trie() = default;

    void insert(const std::u16string &word);

    // 查找以prefix为前缀的所有（或最多limit个，limit==-1表示所有）字符串
    std::vector<std::u16string> searchPrefix(const std::u16string &prefix,
                                             int limit = -1);

    // 持久化存储：保存到文件
    bool save(const std::string &filename) const;

    // 从文件载入
    static Trie load(const std::string &filename);

private:
    std::vector<TrieNode> nodes;

    void dfs(const size_t curr, std::u16string &current,
             std::vector<std::u16string> &result, int limit);

    // 递归序列化节点
    void saveNodes(std::ofstream &ofs) const;

    // 递归反序列化节点
    static Trie loadNodes(std::ifstream &ifs);
};

#endif  // TRIE_H
