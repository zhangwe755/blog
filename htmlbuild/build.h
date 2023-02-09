#include "list.h"

#ifndef HT_BUILD
#define HT_BUILD

typedef struct file_reg_item {
    char *root_file;
    /**
     * 单引用文件列表
     * [sinc_file, sinc_file]
     */
    htlist *sinc_file_list;
    /**
     * 多文件引用列表
     */
    htlist *minc_file_list;
} fileregitem;

typedef struct file_reg {
    /**
     * item key is file_name, value is md5 str for file
     */
    htdict *md5_dict;
    /**
     * dict item value is a point for fileregitem
     */
    htdict *root_dict;
    /**
     * dict item value is a list for fileregitem
     */
    htdict *sfile_dict;
    htdict *mfile_dict;
} filereg;

typedef struct char_node_frag {
    htcharnode *start;
    htcharnode *end;
} charnodefrag;

typedef struct file_result {
    /**
     * 字符串编译节点编译结果
     */
    htcharlist *result;
    char* file_path;
    char* md5;
    /**
     * 变量字段 varName:varValue
     */
    htdict *var_dict;
}fresult;

typedef struct root_context {
    char *tigger_file;

    /**
     * root文件编译结果
     */
    fresult *result;

    /**
     * 单文件编译结果字典
     */
    htdict *sinc_result_dict;

    /**
     * a charnodefrag for mic cmd
     */
    charnodefrag *minc_frag;
    /**
     * list item is a charnodefrag for sinc cmd
     */
    htlist *sinc_frag_list;
    /**
     * 变量字段 varName:varValue
     */
    htdict *var_dict;
    /**
     * 变量引用片段列表
     * [charnodefrag, charnodefrag]
     */
    htlist *var_frag_list;
}rcontext;

#endif


/**
 * 根文件和子文件头部信息描述
 * 每个文件的第一行为头部信息
 * 每个文件的头部信息包含
 *     必选字段 type, path 
 *     自定义可选字段
 * 头部信息格式: type=mark;path=/hello;filedName=xiaoming
 * 必选字段: 
 *     type 可选值: 
 *         root根模版文件
 *         mark自定义类markdown格式文件, child文件格式的一种
 *         html普通html格式文件, child文件格式一种
 *     path: 目标文件路径,一个目标文件最终路径config.destPath + root.path + child.path
 *     filedName: 普通自定义变量名, 字段名可以定义为filedName1, hello等等, 在各个子文件中引用方式 @{rvar:filedName} 或者@{cvar:filedName}
 *
 * root 类型文件包含命令
 *     @{sinc:filepath} 意思是该位置引用filepath的文件内容
 *     @{minc:filedir} 意思是该位置引用该文件夹的所有子文件和子文件夹的所有子文件，可包含多级文件夹
 *                    
 * html 类型文件普通html文件,文件类型直接被root类型文件引用
 * mark 类型定义:
 *     *测试文本*   加粗文本
 *     **测试文本** 斜体
 *     ***测试文本*** 加粗+斜体
 *     [测试文本](http://www.baidu.com) 超文本链接
 *     ![图片描述](http://www.baidu.com/img.jpg)(with:xxx;higth:xxx) 图片
 *
 *     下面的语法是行头有效
 *     # 标题
 *     ## 二级标题
 *     ### 三级标题
 *     #### 四级标题
 *     ##### 五级标题
 *     ###### 六级标题
 *     > 引用文本
 * 
 * 编译过程:
 *     1.编译root文件,所有sinc命令
 *         判断文件是否变更, 变更就编译, 未变更就加载缓存
 *         1.1 生成物: 
 *             root文件变量字典,包含 type path 自定义
 *             生成物字符链表
 *             minc生成物字符链表节点开始结束指针
 *             sinc生成物字符链表节点开始结束指针列表
 *             自定义变量引用列表指针
 *         1.2 生成物缓存
 *             生成物字符链表缓存文件
 *             变量字典缓存文件
 *         1.3 文件注册
 *             注册主文件
 *             注册单个子文件
 *     2.循环编译子文件
 *         判断文件是否变更, 变更就编译, 未变更就加载缓存
 *         1.1 生成物: 
 *             root文件变量字典,包含 type path 自定义
 *             生成物字符链表
 *             自定义变量引用列表指针
 *         1.2 生成物缓存
 *             生成物字符链表缓存文件
 *             变量字典缓存文件
 *         1.3 注册子文件
 *             注册多子文件件
 *     3.自定义变量引用编译
 *     4.目标文件生成
 *
 * 
 * 本函数编译根文件
 * @rootfile 根文件地址
 * @tigger 触发文件地址,如果是子文件地址那么，在编译子文件阶段只编译该子文件
 */
void ht_build(char *rootFile, char *tigger);



