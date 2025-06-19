const Trie = require("../index");

const trie = new Trie();
trie.insert("hello");
trie.insert("helium");
trie.insert("hey");
console.log(trie.searchPrefix("he")); // 输出前缀为 "he" 的单词列表
// 保存trie到文件
console.log("Save result:", trie.save("trie.dat"));
// // 加载trie
// const newTrie = Trie.load("trie.dat");
// // console.log("Load result:", newTrie);
// console.log(newTrie.searchPrefix("he"), "after loading");
// 加载trie
const newTrie = new Trie();
newTrie.load("trie.dat");
// console.log("Load result:", newTrie);
console.log(newTrie.searchPrefix("he"), "after loading");
