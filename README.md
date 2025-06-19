## Usage

git clone this repository and build the project:

```bash
git clone https://github.com/ThXrimson/cpp_persistent_trie.git
```
And in your npm project, install the package:

```bash
npm install <path-to-cpp_persistent_trie>
```

or install from github:

```bash
npm install https://github.com/ThXrimson/cpp_persistent_trie.git
```

Then, in your JavaScript code, you can use commonjs-style imports or ES6-style imports:

```javascript
// CommonJS style
const Trie = require("cpp_persistent_trie");
// ES6 style
import Trie from "cpp_persistent_trie";
```

## Example

```javascript
const Trie = require("cpp_persistent_trie");
const trie = new Trie();
trie.insert("hello");
trie.insert("hell");
trie.insert("你好");
trie.insert("你呢");
console.log(trie.searchPrefix("hello")); // ['hell', 'hello']

trie.save("trie.dat");

const trie2 = new Trie();
trie2.load("trie.dat");
console.log(trie2.searchPrefix("你")); // ['你好', '你呢']
```
