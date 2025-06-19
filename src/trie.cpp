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
    bfs(node, current, result, limit);
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

void Trie::bfs(const TrieNode *node, std::u16string &prefix,
               std::vector<std::u16string> &result, const int limit) {
    auto dq1 = std::deque<std::tuple<const TrieNode *, std::u16string> >();
    auto dq2 = std::deque<std::tuple<const TrieNode *, std::u16string> >();
    dq1.emplace_back(node, prefix.substr(prefix.size() - 1, 1));
    prefix.pop_back();
    while ((limit == -1 || static_cast<int>(result.size()) < limit) && !dq1.empty()) {
        while (!dq1.empty()) {
            const auto curr = dq1.front();
            dq1.pop_front();
            if (std::get<0>(curr)->isWord) {
                result.emplace_back(prefix + std::get<1>(curr));
            }
            for (const auto &[chr, child]: std::get<0>(curr)->children) {
                dq2.emplace_back(child, std::get<1>(curr) + chr);
            }
        }
        std::swap(dq1, dq2);
    }
}

// BFS序列化节点
void Trie::saveNode(BufferWriter &bw, TrieNode *node) {
    auto dq = std::deque<std::tuple<char16_t, TrieNode *> >();
    dq.emplace_back(u'\0', node);
    while (!dq.empty()) {
        const auto curr = dq.front();
        dq.pop_front();
        bw.write(std::get<0>(curr));
        bw.write(std::get<1>(curr)->isWord);
        const auto size = static_cast<uint32_t>(std::get<1>(curr)->children.size());
        bw.write(size);
        for (auto &[chr, child]: std::get<1>(curr)->children) {
            dq.emplace_back(chr, child);
        }
    }
}

// BFS反序列化节点
TrieNode *Trie::loadNode(BufferReader &br) {
    auto dq1 = std::deque<std::tuple<TrieNode *, uint32_t> >();
    auto dq2 = std::deque<std::tuple<char16_t, TrieNode *, uint32_t> >();
    while (!br.eof()) {
        char16_t c;
        br.read(c);
        const auto node = new TrieNode();
        br.read(node->isWord);
        uint32_t size = 0;
        br.read(size);
        dq2.emplace_back(c, node, size);
    }
    TrieNode *res = std::get<1>(dq2.front());
    dq1.emplace_back(std::get<1>(dq2.front()), std::get<2>(dq2.front()));
    dq2.pop_front();
    while (!dq1.empty()) {
        auto curr = std::get<0>(dq1.front());
        auto currSize = std::get<1>(dq1.front());
        dq1.pop_front();
        for (uint32_t i = 0; i < currSize; i++) {
            auto next = dq2.front();
            dq2.pop_front();
            if (std::get<2>(next) > 0) {
                dq1.emplace_back(std::get<1>(next), std::get<2>(next));
            }
            curr->children[std::get<0>(next)] = std::get<1>(next);
        }
    }
    return res;
}
