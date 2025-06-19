#include <napi.h>

#include <string>

#include "trie_compact.h"

class TrieWrapper : public Napi::ObjectWrap<TrieWrapper> {
 public:
  static Napi::Object Init(Napi::Env env, Napi::Object exports);

  TrieWrapper(const Napi::CallbackInfo &info);

 private:
  static Napi::FunctionReference constructor;

  // Trie实例
  Trie trie_;

  // 包装的类方法
  Napi::Value Insert(const Napi::CallbackInfo &info);

  Napi::Value SearchPrefix(const Napi::CallbackInfo &info);

  Napi::Value Save(const Napi::CallbackInfo &info);

  static Napi::Value Load(const Napi::CallbackInfo &info);
};

Napi::FunctionReference TrieWrapper::constructor;

Napi::Object TrieWrapper::Init(Napi::Env env, Napi::Object exports) {
  Napi::HandleScope scope(env);

  Napi::Function func =
      DefineClass(env, "Trie",
                  {InstanceMethod("insert", &TrieWrapper::Insert),
                   InstanceMethod("searchPrefix", &TrieWrapper::SearchPrefix),
                   InstanceMethod("save", &TrieWrapper::Save),
                   StaticMethod("load", &TrieWrapper::Load)});

  constructor = Napi::Persistent(func);
  constructor.SuppressDestruct();

  exports.Set("Trie", func);
  return exports;
}

TrieWrapper::TrieWrapper(const Napi::CallbackInfo &info)
    : Napi::ObjectWrap<TrieWrapper>(info) {
  // 构造函数可以扩展参数，如果需要
}

Napi::Value TrieWrapper::Insert(const Napi::CallbackInfo &info) {
  Napi::Env env = info.Env();

  if (info.Length() < 1 || !info[0].IsString()) {
    Napi::TypeError::New(env, "String expected").ThrowAsJavaScriptException();
    return env.Null();
  }

  std::u16string word = info[0].As<Napi::String>().Utf16Value();
  trie_.insert(word);
  return env.Null();
}

Napi::Value TrieWrapper::SearchPrefix(const Napi::CallbackInfo &info) {
  Napi::Env env = info.Env();

  if (info.Length() < 1 || !info[0].IsString()) {
    Napi::TypeError::New(env, "String expected").ThrowAsJavaScriptException();
    return env.Null();
  }

  std::u16string prefix = info[0].As<Napi::String>().Utf16Value();
  int limit = -1;
  if (info.Length() >= 2 && info[1].IsNumber()) {
    limit = info[1].As<Napi::Number>().Int32Value();
  }

  std::vector<std::u16string> results = trie_.searchPrefix(prefix, limit);

  Napi::Array arr = Napi::Array::New(env, results.size());
  for (size_t i = 0; i < results.size(); ++i) {
    arr.Set(i, Napi::String::New(env, results[i]));
  }
  return arr;
}

Napi::Value TrieWrapper::Save(const Napi::CallbackInfo &info) {
  Napi::Env env = info.Env();

  if (info.Length() < 1 || !info[0].IsString()) {
    Napi::TypeError::New(env, "String filename expected")
        .ThrowAsJavaScriptException();
    return env.Null();
  }

  std::string filename = info[0].As<Napi::String>().Utf8Value();
  bool success = trie_.save(filename);
  return Napi::Boolean::New(env, success);
}

Napi::Value TrieWrapper::Load(const Napi::CallbackInfo &info) {
  Napi::Env env = info.Env();

  if (info.Length() < 1 || !info[0].IsString()) {
    Napi::TypeError::New(env, "String filename expected")
        .ThrowAsJavaScriptException();
    return env.Null();
  }

  std::string filename = info[0].As<Napi::String>().Utf8Value();
  Trie loadedTrie;
  try {
    loadedTrie = Trie::load(filename);
  } catch (const std::exception &e) {
    Napi::Error::New(env, e.what()).ThrowAsJavaScriptException();
    return env.Null();
  }
  // 创建一个空的 JS TrieWrapper对象：
  Napi::Object obj =
      constructor.New({});  // constructor 是 DefineClass 返回的构造器
  // 拿到包装对象指针，替换内部的 trie_
  TrieWrapper *wrapper = Napi::ObjectWrap<TrieWrapper>::Unwrap(obj);
  wrapper->trie_ = std::move(loadedTrie);
  return obj;
}

// 初始化模块
Napi::Object InitAll(Napi::Env env, Napi::Object exports) {
  return TrieWrapper::Init(env, exports);
}

NODE_API_MODULE(trie, InitAll)
