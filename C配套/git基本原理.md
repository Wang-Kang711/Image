## 基本概念
我们先来理解下 Git 工作区、暂存区和版本库概念：

- 工作区：就是你在电脑里能看到的目录。
- 暂存区：英文叫 stage 或 index。一般存放在 .git 目录下的 index 文件（.git/index）中，所以我们把暂存区有时也叫作索引（index）。
- 版本库：工作区有一个隐藏目录 .git，这个不算工作区，而是 Git 的版本库。
  
下面这个图展示了工作区、版本库中的暂存区和版本库之间的关系：
![20220716202823](https://raw.githubusercontent.com/Wang-Kang711/Image/main/common/20220716202823.png)

---


## 核心原理
![20220716202048](https://raw.githubusercontent.com/Wang-Kang711/Image/main/common/20220716202048.png)

> - 工作区中通过add添加到暂缓区用于追踪(索引)，untracked
> - 暂缓区中通过commmit添加到本地仓库用于实际版本控制，tracked(stage)
> - 本地仓库中通过push添加到远程仓库用于同步
> - pull会直接更新到工作区，并覆盖工作区内容。因此可以首先用fetch更新本地仓库，再用diff对比区别，没问题在用于合并。

---


## git本地操作
### 配置用户信息
```git
git config --global user.name "wk"
git config --global user.email "XX@163.com"
```

### 初始化文件夹
对文件夹初始化，表示该文件夹需要用于git管理
```git
git init
```
创建出隐藏文件夹.git，用于存放所有文件的版本及索引。
处理化文件夹后，一般默认在master分支。

### 查看文件状态
查看文件状态，查看哪些文件用于git管理
```git
git status
```
![20220716205330](https://raw.githubusercontent.com/Wang-Kang711/Image/main/common/20220716205330.png)
> 第一行表示当前所在分支，黄框为未追踪文件(untracked)，即不用于git管理

### 追踪文件(将文件添加至缓存区)
文件夹中所有.md文件 添加进缓存区
```git
git add *.md
```
此时再查看文件状态，则已经用于追踪
![20220716210055](https://raw.githubusercontent.com/Wang-Kang711/Image/main/common/20220716210055.png)

###提交至本地仓库
提交为一个固定版本。
```git
// -m为跳过vim编写提交说明。
git commit -m "fix(test): change content"
```
提交说明可参考[commit提交风格](https://andyli.blog.csdn.net/article/details/86723431?spm=1001.2101.3001.6661.1&utm_medium=distribute.pc_relevant_t0.none-task-blog-2%7Edefault%7ECTRLIST%7Edefault-1-86723431-blog-122706900.pc_relevant_multi_platform_whitelistv1_exp2&depth_1-utm_source=distribute.pc_relevant_t0.none-task-blog-2%7Edefault%7ECTRLIST%7Edefault-1-86723431-blog-122706900.pc_relevant_multi_platform_whitelistv1_exp2&utm_relevant_index=1)

此时查看状态信息，工作区已被清空
![20220716211422](https://raw.githubusercontent.com/Wang-Kang711/Image/main/common/20220716211422.png)

> **一种特殊状态**
> 当新增后从工作区add到暂缓区后，若对工作区再添加内容，此时查看文件状态。
> ![20220716211916](https://raw.githubusercontent.com/Wang-Kang711/Image/main/common/20220716211916.png)
> 此时存在两种状态，即后续可以commit，或者add(restore)。
> 第一次add后则可以commit，但由于又添加了内容，因此工作区需要将新内容添加到暂缓区。
> 若此时直接commit，则提交的是第一次add的内容。
> **常见的手段**是将工作区内容add到暂缓区，再一次commit提交。

### 版本查看
```git
git log
```
可见共两个版本，9b68b5376eb3fb54d574a5715249c1312a9f6b1c为唯一随机hash编号。
![20220716212720](https://raw.githubusercontent.com/Wang-Kang711/Image/main/common/20220716212720.png)

### 版本回退
```git
git reset --hard/--soft/--mixed(default) [head]

--mixed 为默认，用于重置暂存区的文件与上一次的提交(commit)保持一致，工作区文件内容保持不变。
--hard 撤销工作区中所有未提交的修改内容，将暂存区与工作区都回到上一次版本，并删除之前的所有信息提交
--soft 索引（暂存区）和工作区不变

// head 参数
$ git reset HEAD^              # 回退所有内容到上一个版本  
$ git reset HEAD^ hello.php    # 回退 hello.php 文件的版本到上一个版本  
$ git  reset  hashID           # 回退到指定版本
```

### 文件忽略gitignore
> 为什么需要忽略文件？
> 一个常见的原因是 例如C文件 需要编译与运行等步骤，产生一系列中间文件。而文件管理只在意源文件。更重要的原因是如Java需要依赖一些的工具库，需将工具排除文件版本管理，否则会引入大量无关文件。
```shell
$vim .gitignore

// .gitignore中忽略jpg文件。
*.jpg
```

### **分支**
#### 分支创建切换
```git
$ git branch bad-boy // 创建分支
$ git checkout bad-boy // 切换到bad-boy分支

$ git checkout -b bad-boy // 创建并切换到bad-boy分支
$ git branch // 查看所有分支
```

#### 分支删除
```git
$ git branch -d bad-boy // 删除分支，若未合并则提示
$ git checkout -D bad-boy // 直接删除分支
```
需注意 .gitignore中排除git管理的文件，若branch中删除，则master也无法回复。

#### 分支合并
```git
$ git merge bad-boy // 当前分支与bad-boy分支合并
```
可能因版本冲突(行冲突)，需手动处理





