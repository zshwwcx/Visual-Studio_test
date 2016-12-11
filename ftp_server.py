import sys,os,time
from socket import *
import _thread as thread

myHost=''
myPort=50007
syn_port=50010
slaveryPort=8888
buff=1024
def file_read():
    
    dir=os.listdir()
    for file_name in dir:
        file_list.append(file_name)


def serverthread(client_count,address):
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


def server_synchronization():
    file_syn_list=os.listdir()
    sock_server=socket(AF_INET,SOCK_STREAM)
    sock_server.bind((myHost,syn_port))
    sock_server.listen(5)
    pass
    while True:
        time.sleep(3)
        syn_connection,syn_address=sock_server.accept()
        file_recv_list=sock_server.recv(buff).decode().split()[1:-1]
        for file_syn in file_recv_list:
            if file_syn not in file_recv_list:
                syn_connection.send(('Start uploading file: %s'%file_syn).encode())
                thread.start_new_thread(syn_upload,(syn_connection,))
                time.sleep(10)
        

def syn_upload(syn_connection):
    sock_send=socket(AF_INET,SOCK_STREAM)
    pass

def syn_download(syn_connection):
    sock_receive=socket(AF_INET,SOCK_STREAM)
    #sock_receive.bind()
    pass

    
sock=socket(AF_INET,SOCK_STREAM)
sock.bind((myHost,myPort))
sock.listen(25)

server_pool={1:{'host':'localhost','port':8880},2:{'host':'localhost','port':8881},3:{'host':'localhost','port':8882},4:{'host':'localhost','port':8883},5:{'host':'localhost','port':8884},}

client_count=0
client_ip_dict={}
file_list=[]
while True:
    time.sleep(0.1)
    connection,address=sock.accept()
    client_count+=1
    print('Server %s connected by '%myHost,address)
    client_ip=str(address).split(',')[0]
    if client_ip in client_ip_dict:
        client_ip_dict[client_ip]+=1
    else:
        client_ip_dict[client_ip]=1
    print('Client count:%d \n'%client_count)
    connection.send(('Now server time is : %s \n Login ip is : %s \n----------------------------\n'%(time.ctime(time.time()),address)).encode())
    connection.send(('Client numbers connecting to the server now is %d \n----------------------------\n'%client_count).encode())
    connection.send(('This is your %d times to connect to the server,\n----------------------------\n'%client_ip_dict[client_ip]).encode())
    connection.send(('host_name: %s ,port_name: %s \n----------------------------\n'%(server_pool[(client_count%5)]['host'],server_pool[(client_count%5)]['port'])).encode())
    file_read()
    connection.send(('The file on the server is:\n %s \n'%str(file_list)).encode())
    thread.start_new_thread(serverthread,(client_count,address,))
    """
    while True:
        data=connection.recv(buff)
        if not data:
            break
        print(data.decode())
    """
connection.close()
client_count-=1
        