#include "trie_compact.h"

#include <cstring>
#include <fstream>
#include <string>
#include <unordered_map>
#include <vector>

void Trie::insert(const std::u16string &word) {
    size_t curr = 0;
    for (const char16_t c: word) {
        if (!nodes[curr].children.count(c)) {
            nodes[curr].children[c] = nodes.size();
            nodes.emplace_back();
        }
        curr = nodes[curr].children[c];
    }
    nodes[curr].isWord = true;
}

// 查找以prefix为前缀的所有（或最多limit个，limit==-1表示所有）字符串
std::vector<std::u16string> Trie::searchPrefix(const std::u16string &prefix,
                                               const int limit) {
    std::vector<std::u16string> result;
    size_t curr = 0;
    for (char16_t c: prefix) {
        if (!nodes[curr].children.count(c)) return result;
        curr = nodes[curr].children[c];
    }
    std::u16string current = prefix;
    dfs(curr, current, result, limit);
    return result;
}

// 持久化存储：保存到文件
bool Trie::save(const std::string &filename) const {
    std::ofstream ofs(filename, std::ios::binary);
    if (!ofs) return false;
    saveNodes(ofs);
    return true;
}

// 从文件载入
Trie Trie::load(const std::string &filename) {
    std::ifstream ifs(filename, std::ios::binary);
    if (!ifs) throw std::runtime_error("File not found");
    return loadNodes(ifs);
}

void Trie::dfs(const size_t curr, std::u16string &current,
               std::vector<std::u16string> &result, const int limit) {
    if (limit != -1 && static_cast<int>(result.size()) >= limit) return;
    if (nodes[curr].isWord) result.push_back(current);
    for (const auto &[chr, node]: nodes[curr].children) {
        current.push_back(chr);
        dfs(node, current, result, limit);
        current.pop_back();
        if (limit != -1 && static_cast<int>(result.size()) >= limit) return;
    }
}

// 序列化节点
void Trie::saveNodes(std::ofstream &ofs) const {
    const size_t nodesSize = nodes.size();
    ofs.write(reinterpret_cast<const char *>(&nodesSize), sizeof(size_t));
    size_t sumSize = 0;
    const auto nodesList = new char[nodesSize * (sizeof(bool) + sizeof(size_t))];
    size_t pos = 0;
    for (const auto &[isWord, children]: nodes) {
        memcpy(nodesList + pos, &isWord, sizeof(bool));
        pos += sizeof(bool);
        const size_t childrenSize = children.size();
        memcpy(nodesList + pos, &childrenSize, sizeof(size_t));
        pos += sizeof(size_t);
        sumSize += childrenSize;
    }
    ofs.write(nodesList, nodesSize * (sizeof(bool) + sizeof(size_t)));
    delete[] nodesList;
    const auto childrenList = new char[(sizeof(char16_t) + sizeof(size_t)) * sumSize];
    pos = 0;
    for (const auto &[_, children]: nodes) {
        for (const auto &[chr, node]: children) {
            memcpy(childrenList + pos, &chr, sizeof(char16_t));
            pos += sizeof(char16_t);
            memcpy(childrenList + pos, &node, sizeof(size_t));
            pos += sizeof(size_t);
        }
    }
    ofs.write(childrenList, (sizeof(char16_t) + sizeof(size_t)) * sumSize);
    delete[] childrenList;
}

// 反序列化节点
Trie Trie::loadNodes(std::ifstream &ifs) {
    size_t nodesSize = 0;
    ifs.read(reinterpret_cast<char *>(&nodesSize), sizeof(size_t));

    std::vector<TrieNode> nodes(nodesSize);

    const auto childrenSizes = new size_t[nodesSize];
    size_t sumSize = 0;

    const auto nodesList = new char[nodesSize * (sizeof(bool) + sizeof(size_t))];
    size_t pos = 0;
    ifs.read(nodesList, nodesSize * (sizeof(bool) + sizeof(size_t)));
    for (size_t i = 0; i < nodesSize; i++) {
        size_t childrenSize = 0;
        memcpy(&nodes[i].isWord, nodesList + pos, sizeof(bool));
        pos += sizeof(bool);
        memcpy(&childrenSize, nodesList + pos, sizeof(size_t));
        pos += sizeof(size_t);
        nodes[i].children.reserve(childrenSize);
        childrenSizes[i] = childrenSize;
        sumSize += childrenSize;
    }
    delete[] nodesList;

    const auto childrenList = new char[(sizeof(char16_t) + sizeof(size_t)) * sumSize];
    pos = 0;
    ifs.read(childrenList, (sizeof(char16_t) + sizeof(size_t)) * sumSize);
    for (size_t i = 0; i < nodesSize; i++) {
        auto &[_, children] = nodes[i];
        for (size_t j = 0; j < childrenSizes[i]; j++) {
            char16_t chr;
            memcpy(&chr, childrenList + pos, sizeof(char16_t));
            pos += sizeof(char16_t);
            size_t node;
            memcpy(&node, childrenList + pos, sizeof(size_t));
            pos += sizeof(size_t);
            children[chr] = node;
        }
    }
    delete[] childrenList;
    delete[] childrenSizes;
    return Trie(move(nodes));
}
