## 使用chrome的技巧

#### 避开HSTS
- chrome可以通过配置chrome://net-internals/#hsts 规避检查，在删除除添加域名，然后再query，如果结果是not found 那么就可以了

#### 关闭xss filter
- 终端启动 --args --disable-xss-auditor