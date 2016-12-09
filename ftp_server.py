import sys,os,time
from socket import *
import _thread as thread

myHost=''
myPort=50007
slaveryPort=8888
buff=1024
def file_read():
    dir=os.listdir()
    for file_name in dir:
        file_list.append(file_name)


def serverthread(connection):
    sock1=socket(AF_INET,SOCK_STREAM)
    sock1.bind((myHost,slaveryPort))
    sock1.listen(5)
    while True:
        connection_1,address_1=sock1.accept()
        filename=connection_1.recv(buff).split()[0]
        file_upload=open(r'F:\Code\pointtest\ftp_gui\%s'%filename,'wb')#Where the file server received stored
        data=connection_1.recv(buff)
        if not data:
            break
        file_upload.write(data)
    sock1.close()
    print('Server got file %s.\n'%filename)
    file_upload.close()

    connection.close()

sock=socket(AF_INET,SOCK_STREAM)
sock.bind((myHost,myPort))
sock.listen(5)

file_list=[]
client_count=0

while True:
    connection,address=sock.accept()
    client_count+=1
    print('Server %s connected by '%myHost,address)
    print('Client count:%d \n'%client_count)
    connection.send(('Now server time is : %s \n Login ip is : %s \n'%(time.ctime(time.time()),address)).encode())
    connection.send(('Client connecting to the server now is %d \n'%client_count).encode())
    file_read()
    connection.send(('The file on the server is:\n %s \n'%str(file_list)).encode())
    thread.start_new_thread(serverthread,(connection,))
    """
    while True:
        data=connection.recv(buff)
        if not data:
            break
        print(data.decode())
    """
connection.close()
client_count-=1
        