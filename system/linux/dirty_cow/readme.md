##cve编号：CVE-2016-5195

##漏洞名称：提权漏洞
##漏洞类型：本地提权
##影响版本：
- 影响版本很多，所以列出已经修复吃的版本
### fixed system
#### ubuntu
- 4.8.0-26.28 for Ubuntu 16.10
- 4.4.0-45.66 for Ubuntu 16.04 LTS
- 3.13.0-100.147 for Ubuntu 14.04 LTS
- 3.2.0-113.155 for Ubuntu 12.04 LTS
#### Debian
- 3.16.36-1+deb8u2 for Debian 8
- 3.2.82-1 for Debian 7
- 4.7.8-1 for Debian unstable
#### Arch
- 4.4.26-1 for ArchLinux (linux-lts package)
- 4.8.3 for ArchLinux (linux package)
#### Gentoo
- sys-kernel/gentoo-sources-4.1.35
- sys-kernel/gentoo-sources-4.8.x - TBD
- sys-kernel/gentoo-sources-3.x.x - TBD
#### others
- If you must stay in kernel 3.19 family, version 3.19.0-73.81 is a patched version and available for Ubuntu 14.04 from official repos. (update via dist-upgrade)
##利用脚本：
- [dirtycow](https://github.com/dirtycow/dirtycow.github.io.git)
- 检查是否受影响：the dirtyc0w.c exploit
##漏洞细节：
##reference：
- [detail](https://github.com/dirtycow/dirtycow.github.io/wiki/VulnerabilityDetails)