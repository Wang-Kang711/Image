#  chmod

## 文件系统

![image-20220701120025267](https://raw.githubusercontent.com/Wang-Kang711/Image/main/common/202207011200296.png)

分别代表

- 文件类型及权限[权限 `r`(read读)、`w`(write写)、`x`(execute执行)]
- 链接数
- 所有者
- 所属组
- 文件大小
- 最后修改日期
- 文件名称

单看权限这一项分为四个部分，从左向右分别为

- 文件类型 `d`(directory目录)、`-`(文件)
- 文件所有者权限(从左到右分别为读、写、可执行权限，`-`表示无此权限)
- 同组用户权限
- 非同组用户权限

## 用法

```
chmod [u、g、o、a] [+(加入)、-(除去)、=(设置)] [r、w、x] 文件名
```

- `u`:user(所有者)
- `g`:group(同组用户)
- `o`:other(其他)
- `a`:all(所有)

# chown

## 文件访问权限

通过 `stat` 可获取某个文件的 mode。

`mode` 指 linux 中对某个文件的访问权限。

```shell
# -c：--format
# %a：获得数字的 mode
# %A：获得可读化的 mode
$ stat -c %a README.md
644
$ stat -c %A README.md 
-rw-r--r--
```

## 用法

```shell
chown [options] user[:group] file(s)
```

常用选项如下：

- `-c`：效果类似`-v`参数，但仅显示更改部分的信息
- `-f`：忽略错误信息
- `-h`：只对符号连接的文件作修改，而不更改其他任何相关文件
- `-R`：递归处理，处理指定目录以及其子目录下的所有文件
- `-v`：显示详细的处理信息

示例

```sh
# 将 . 文件夹下main.c文件的用户及用户组设为 wk
# -R：遍历子文件修改
$ chown -R wk:wk main.c
```

