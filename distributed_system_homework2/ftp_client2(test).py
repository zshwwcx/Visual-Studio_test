import sys,os,time
from socket import *
server_host='localhost'
server_port=50007
buff=1024

sock=socket(AF_INET,SOCK_STREAM)
sock.connect((server_host,server_port))

while True:
    data=sock.recv(buff)
    if not data:
        break
    print(data.decode())
sock.close()
