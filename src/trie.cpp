#include "trie.h"

#include <cstdint>
#include <deque>
#include <fstream>
#include <string>
#include <map>
#include <vector>
#include <iostream>

void Trie::insert(const std::u16string &word) {
    if (root == nullptr) {
        root = new TrieNode();
    }
    TrieNode *cur = root;
    for (char16_t c: word) {
        if (!cur->children.count(c)) {
            cur->children[c] = new TrieNode();
        }
        cur = cur->children[c];
    }
    cur->isWord = true;
}

// 查找以prefix为前缀的所有（或最多limit个，limit==-1表示所有）字符串
std::vector<std::u16string> Trie::searchPrefix(const std::u16string &prefix,
                                               const int limit) const {
    std::vector<std::u16string> result;
    TrieNode *node = root;
    for (char16_t c: prefix) {
        if (!node->children.count(c)) return result;
        node = node->children[c];
    }
    std::u16string current = prefix;
    dfs(node, current, result, limit);
    return result;
}

// 持久化存储：保存到文件
bool Trie::save(const std::string &filename) const {
    std::ofstream ofs(filename, std::ios::binary);
    if (!ofs) return false;
    BufferWriter bw;

    saveNode(bw, root);

    bw.flush(ofs);

    return true;
}

// 从文件载入
bool Trie::load(const std::string &filename) {
    std::ifstream ifs(filename, std::ios::binary | std::ios::ate);
    if (!ifs) return false;
    const auto fileSize = ifs.tellg();
    ifs.seekg(0, std::ios::beg);
    std::vector<char> buffer(fileSize);

    if (!ifs.read(buffer.data(), fileSize)) return false;

    BufferReader br(buffer.data(), buffer.size());
    delete root;
    root = nullptr;
    root = loadNode(br);

    return true;
}

void Trie::dfs(const TrieNode *node, std::u16string &prefix,
               std::vector<std::u16string> &result, const int limit) {
    if (limit != -1 && result.size() == limit) return;
    if (node->isWord) {
        result.push_back(prefix);
    }
    for (auto &[chr, child]: node->children) {
        prefix.push_back(chr);
        dfs(child, prefix, result, limit);
        prefix.pop_back();
        if (limit != -1 && result.size() == limit) return;
    }
}

// DFS序列化节点
void Trie::saveNode(BufferWriter &bw, TrieNode *node) {
    bw.write(node->isWord);
    const uint32_t size = node->children.size();
    bw.write(size);
    for (auto &[chr, child]: node->children) {
        bw.write(chr);
        saveNode(bw, child);
    }
}

// DFS反序列化节点
TrieNode *Trie::loadNode(BufferReader &br) {
    const auto node = new TrieNode();
    br.read(node->isWord);
    uint32_t size;
    br.read(size);
    for (uint32_t i = 0; i < size; i++) {
        char16_t chr;
        br.read(chr);
        node->children[chr] = loadNode(br);
    }
    return node;
}
