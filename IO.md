`input` & `output`, 是一切实现的基础

- `sysio`：系统IO

- `stdio`：标准IO，依赖于系统调用IO。
  
  - 优先使用，移植性好
  
  - 合并系统调用，`buffer`和`cache`机制，读写加速？

---

## fopen

```c
FILE *fopen(const char *pathname, const char *mode);
RETURN VALUE
       Upon successful completion fopen(), fdopen() and freopen()
       return a FILE pointer.  Otherwise, NULL is returned and errno 
       is set to indicate the error.
```

| 模式  | 功能   | 特点                 |
| --- | ---- | ------------------ |
| r   | 只读   | 指针位于头，不检查文件是否存在    |
| r+  | 读与写  |                    |
| w   | 只写   | 有则清空，无则创建，指针位于头    |
| w+  | 读与写  |                    |
| a   | 追加   | 在最后一位有效字符下一位，指针位于尾 |
| a+  | 读与追加 | 追加指针位于尾，读指针位于头     |

### 显示系统错误信息的方式

`errno`——默认以**全局变量形式**存在，当发生错误则输出到`errno`中。但是注意若不及时输出则会被其余**错误信息覆盖**。

**errno错误码**，参照如下

```c
> vim  /usr/include/asm-generic/errno-base.h

/* SPDX-License-Identifier: GPL-2.0 WITH Linux-syscall-note */
#ifndef _ASM_GENERIC_ERRNO_BASE_H
#define _ASM_GENERIC_ERRNO_BASE_H

#define EPERM            1      /* Operation not permitted */
#define ENOENT           2      /* No such file or directory */
#define ESRCH            3      /* No such process */
#define EINTR            4      /* Interrupted system call */
#define EIO              5      /* I/O error */
#define ENXIO            6      /* No such device or address */
#define E2BIG            7      /* Argument list too long */
#define ENOEXEC          8      /* Exec format error */
#define EBADF            9      /* Bad file number */
#define ECHILD          10      /* No child processes */
#define EAGAIN          11      /* Try again */
#define ENOMEM          12      /* Out of memory */
#define EACCES          13      /* Permission denied */
#define EFAULT          14      /* Bad address */
#define ENOTBLK         15      /* Block device required */
#define EBUSY           16      /* Device or resource busy */
#define EEXIST          17      /* File exists */
#define EXDEV           18      /* Cross-device link */
#define ENODEV          19      /* No such device */
#define ENOTDIR         20      /* Not a directory */
#define EISDIR          21      /* Is a directory */
#define EINVAL          22      /* Invalid argument */
#define ENFILE          23      /* File table overflow */
#define EMFILE          24      /* Too many open files */
#define ENOTTY          25      /* Not a typewriter */
#define ETXTBSY         26      /* Text file busy */
#define EFBIG           27      /* File too large */
#define ENOSPC          28      /* No space left on device */
#define ESPIPE          29      /* Illegal seek */
#define EROFS           30      /* Read-only file system */
#define EMLINK          31      /* Too many links */
#define EPIPE           32      /* Broken pipe */
#define EDOM            33      /* Math argument out of domain of func */
#define ERANGE          34      /* Math result not representable */

#endif
```

但若直接答应errno，则会输出一个整数。

- 采用`perror`自动关联于**error**与**sys_nerr**

```c
#include <stdio.h>

       void perror(const char *s);

       #include <errno.h>

       const char * const sys_errlist[];
       int sys_nerr;
       int errno;       /* Not really declared this way; see errno(3) */
```

- 采用`strerror`

```c
/* 原型 */
char *strerror(int errnum);

/*  常用方式,将其写入到错误输出流中  */
fprintf(stderr, "fopen():%s\n", strerror(errno));
```

---

## File结构体储存在哪

- 栈。不可能，若为栈在初始化返回则为局部变量，返回时已经销毁无法继续调用。

- 静态区。不对，当为静态区是，每次调用只有第一次初始化，后续调用占用同一块空间。此时无法打开两个文件，且生命周期长，无法`fclose`。

- 堆。正确。

---

## 常用规则

- 谁打开谁关闭

- 谁申请谁释放

- **资源一定有上限**

### 文件打开资源上限为多少？

```c
int main(void){
    FILE *fp;
    int count = 0;

    while(1)
    {    
        fp = fopen("tmp","r");
        if(fp==NULL){
            perror("fopen()");
            break;
        }
        count++;
    }

    printf("count = %d\n",count);

    // fclose(fp);
    exit(0);
}

> 65532
```

为何最大为65532？

进程默认打开3个流：`stdio/stdout/stderr`；

因此最大限制应该为`1021 + 3 = 65532`。当然堆也是其中限制之一。

### 查看/修改最大限制

```shell
root@riptide:~# ulimit -a
core file size          (blocks, -c) 0
data seg size           (kbytes, -d) unlimited
scheduling priority             (-e) 0
file size               (blocks, -f) unlimited
pending signals                 (-i) 7702
max locked memory       (kbytes, -l) 65536
max memory size         (kbytes, -m) unlimited
open files                      (-n) 65535
pipe size            (512 bytes, -p) 8
POSIX message queues     (bytes, -q) 819200
real-time priority              (-r) 0
stack size              (kbytes, -s) 8192
cpu time               (seconds, -t) unlimited
max user processes              (-u) 7702
virtual memory          (kbytes, -v) unlimited
file locks                      (-x) unlimited
```

其中open files用于限制最大打开个数，默认2Byte用于管理文件流打开个数

**修改**：

```shell
# 修改打开文件个数
ulimit -n 1024
# 修改管道数量
#ulimit -p 16 
```

使用fopen的w/a模式会创建文件，那么文件权限是多小那？

![](C:\Users\yrjak\AppData\Roaming\marktext\images\2022-08-07-14-32-11-image.png)

> 664。为何？
> 
> 文件创建时候，采用w模式。默认为`0x0666 &  ~(umask)`
> 
> 可以修改umask值从而限制生成文件权限的问题。
> 
> ```c
> root@riptide:~# umask
> 0002
> ```


