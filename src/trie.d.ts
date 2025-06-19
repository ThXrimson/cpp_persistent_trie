export class Trie {
  /**
   * 构造函数，当前无参数
   */
  constructor();

  /**
   * 插入一个字符串到Trie中
   * @param word 要插入的字符串
   */
  insert(word: string): void;

  /**
   * 搜索以给定前缀开头的字符串
   * @param prefix 前缀字符串
   * @param limit 最多返回多少结果，默认不限制
   * @returns 符合前缀的字符串数组
   */
  searchPrefix(prefix: string, limit?: number): string[];

  /**
   * 将Trie保存到文件
   * @param filename 文件路径
   * @returns 保存是否成功
   */
  save(filename: string): boolean;

  /**
   * 从文件加载Trie
   * @param filename 文件路径
   * @returns 加载是否成功
   */
  load(filename: string): boolean;
}
