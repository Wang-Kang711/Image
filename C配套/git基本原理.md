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


## git实操
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
> 第一行表示当前所在分支，黄框为未追踪文件，即不用于git管理




