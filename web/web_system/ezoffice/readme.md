##cve编号：无
##漏洞名称：上传文件导致代码执行
##漏洞类型：上传文件导致代码执行
##影响版本：

##利用脚本：
##漏洞细节：
- 上传到uploadFolder/path/fileId.substring(0, 6)/fileId
- xxe漏洞
- jmx-console 存在默认口令: admin/ezoffice
- webservice服务需要一个通信密码，但官方自己留了一个万能密码：auth.key.whir2012
##reference：
