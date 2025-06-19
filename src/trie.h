#pragma once
#include <cstring>
#include <fstream>
#include <memory>
#include <string>
#include <map>
#include <vector>


class BufferWriter {
public:
    std::vector<char> buffer;

    template<typename T>
    void write(const T &data) {
        const auto p = reinterpret_cast<const char *>(&data);
        buffer.insert(buffer.end(), p, p + sizeof(T));
    }

    void flush(std::ofstream &ofs) {
        ofs.write(buffer.data(), buffer.size());
        buffer.clear();
    }
};

class BufferReader {
    const char *data; // 指向缓冲区内存首地址
    size_t size; // 缓冲区大小
    size_t pos; // 当前读取位置
public:
    BufferReader(const char *_data, const size_t _size) : data(_data), size(_size), pos(0) {
    }

    // 读取一个指定类型T的数据，T必须字节连续
    template<typename T>
    void read(T &out) {
        if (pos + sizeof(T) > size) {
            throw std::runtime_error("BufferReader reached end");
        }
        std::memcpy(&out, data + pos, sizeof(T));
        pos += sizeof(T);
    }

    // 判断是否到结尾
    [[nodiscard]] bool eof() const {
        return pos >= size;
    }
};

// Trie节点定义
struct TrieNode {
    bool isWord = false;
    std::map<char16_t, TrieNode *> children;

    ~TrieNode() {
        for (auto &[_, child]: children) {
            delete child;
        }
    }
};

class Trie {
public:
    Trie(): root(new TrieNode()) {
    };

    ~Trie() {
        delete root;
    }

    void insert(const std::u16string &word);

    // 查找以prefix为前缀的所有（或最多limit个，limit==-1表示所有）字符串
    std::vector<std::u16string> searchPrefix(const std::u16string &prefix,
                                             int limit = -1) const;

    // 持久化存储：保存到文件
    bool save(const std::string &filename) const;

    // 从文件载入
    bool load(const std::string &filename);

private:
    TrieNode *root;

    static void dfs(const TrieNode *node, std::u16string &prefix,
                          std::vector<std::u16string> &result, int limit);

    // 递归序列化节点
    static void saveNode(BufferWriter &bw, TrieNode *node);

    // 递归反序列化节点
    static TrieNode *loadNode(BufferReader &br);
};
