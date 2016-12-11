import os

parm=0
while True:
    parm+=1
    pid=os.fork()
    if pid==0:
        os.execlp('python','python','child.py',str(parm))
        assert False,'error in starting the program'
    else:
        print('child is ',pid)
        if input()==q:
            break       