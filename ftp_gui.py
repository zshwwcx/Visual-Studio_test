import sys,os,time
from socket import *
from tkinter import *
from tkinter.messagebox import showinfo
import _thread as thread

server_host='localhost'
server_port=50007
buff=1024
slavery_host='localhost'
slavery_port=8888


def show_time_now():
    return time.ctime(time.time())

def client_func(sock1):
    sock1.connect((server_host,server_port))
    while True:
        data=sock1.recv(buff)
        if not data:
            break
        print(data.decode())
    sock1.close()
    

def client_upload():
    def upload_fetch():
        value=ent.get()
        sock1=socket(AF_INET,SOCK_STREAM)
        def upload_success():
            root2=Tk()
            success_label=Label(root2,text=('file %s has been uploaded successfully.\n'%value))
            success_label.pack()
            root2.mainloop()
        #upload_success()
       
        sock1.connect((slavery_host,slavery_port))
        if value not in os.listdir():
            print('Error:No such file in the directory.\n')
        else:
            sock1.send((value+'\n').encode())
            file_send=open(value,'rb')
            while True:
                bytes=file_send.read(buff)
                if not bytes:
                    break
                sock1.send(bytes)
            sock1.close()

        

    root1=Tk()
    ent=Entry(root1)
    ent.insert(0,'')
    ent.pack(side=TOP,fill=X)
    ent.focus()
    ent.bind('<Return>',(lambda event:upload_fetch()))
    fetch_button=Button(root1,text='fetch',command=upload_fetch)
    fetch_button.pack(side=LEFT)
    quit_button=Button(root1,text='Quit',command=sys.exit)
    quit_button.pack(side=RIGHT)
    root1.mainloop()

def client_download():
    def download_fetch():
        value=ent.get()
        sock1=socket(AF_INET,SOCK_STREAM)

        def download_success():
            root2=Tk()
            success_label=Label(root2,text=('file %s has been downloaded successfully.\n'%value))
            success_label.pack()
            root2.mainloop()
        #download_success()

        
    root1=Tk()
    ent=Entry(root1)
    ent.insert(0,'')
    ent.pack(side=TOP,fill=X)
    ent.focus()
    ent.bind('<Return>',(lambda event:upload_fetch()))
    fetch_button=Button(root1,text='fetch',command=download_fetch)
    fetch_button.pack(side=LEFT)
    quit_button=Button(root1,text='Quit',command=sys.exit)
    quit_button.pack(side=RIGHT)
    root1.mainloop()


sock=socket(AF_INET,SOCK_STREAM)
root=Tk()
root.title('ftp_gui')

#show welcome infomation in the gui
welcome_label=Label(root,text='Welcome for using ftp_test app.')
welcome_label.grid(row=0,column=0)

#show time in the gui
time_label=Label(root,text='Time:')
time_label.grid(row=1,column=0)
time_now=show_time_now()
now_label=Label(root)
now_label.config(text=time_now)
now_label.grid(row=1,column=1)

#The button show in gui
login_button=Button(root,text='Login',command=(lambda:thread.start_new_thread(client_func,(sock,))))
login_button.grid(row=2,column=2)

upload_button=Button(root,text='Upload',command=client_upload)
upload_button.grid(row=2,column=3)

download_button=Button(root,text='Download',command=client_download)
download_button.grid(row=2,column=4)

exit_button=Button(root,text='Exit',command=sys.exit)
exit_button.grid(row=2,column=5)

root.mainloop()
