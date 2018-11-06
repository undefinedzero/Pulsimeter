#-*-coding:utf-8-*-
import math
import matplotlib.pyplot as plt
import matplotlib.animation as animation
import time 
import sys
import paramiko
import numpy as np

def ssh_connect( _host, _username, _password ):
	try:
		_ssh_fd = paramiko.SSHClient()
		_ssh_fd.set_missing_host_key_policy( paramiko.AutoAddPolicy() )
		_ssh_fd.connect( _host, username = _username, password = _password )
	except Exception, e:
		print( 'ssh %s@%s: %s' % (_username, _host, e) )
		exit()
	return _ssh_fd

def sftp_open( _ssh_fd ):
	return _ssh_fd.open_sftp()

def sftp_put( _sftp_fd, _put_from_path, _put_to_path ):
	return _sftp_fd.put( _put_from_path, _put_to_path )

def sftp_get( _sftp_fd, _get_from_path, _get_to_path ):
	return _sftp_fd.get( _get_from_path, _get_to_path )

def sftp_close( _sftp_fd ):
	_sftp_fd.close()

def ssh_close( _ssh_fd ):
	_ssh_fd.close()

def Healthcare(roads):
	data = np.loadtxt(roads,delimiter = ',')
	h=[]
	for i in range(len(data)):
		if(i<len(data)-1):
			if data[i+1] - data[i]<=0 and data[i] - data[i-1] >=0:
				if not (data[i+1] - data[i]==0 and data[i] - data[i-1] ==0):
					h.append([data[i],i])
	h.sort(reverse=True) #h is the x axis
	#====================================
	h0=np.array(h)
	h0=h0[:8]
	h0=h0[np.lexsort(h0.T)]#inverse queue
	#trun invalid number to [-1,-1]
	for i in range(len(h0)):
		if(i<len(h0)-1):
			if h0[i+1][1]-h0[i][1]==1:
				h0[i]=[-1,-1]

	#select top view  --> y0
	y0=[]
	for i in range(len(h0)):
		if(i<len(h0)-1):
			if abs(h0[i][0] - np.max([x[0] for x in h0]))< 10: #high tops
				y0.append(h0[i])
	#trun x0 as intervail		
	x0=[]
	for i in range(len(y0)):
		if i<len(y0)-1:
			x0.append(y0[i+1][1]-y0[i][1])
	#====================================
	#[x[1] for x in h0] select one colunm in it
	f_1=np.average(x0)
	f=60/(f_1*0.02)
	print ("心律：")
	print f
	health=True
	#justice-1
	print ("心脏健康状况：")
	if f > 100:
		health=False
		print("窦性心动过速！")
	else:
		if f < 60:
			health=False
			print("窦性心动过缓！")
	#justice-2
	c0=[]
	for i in range(len(x0)):
		if i<len(x0)-1:
		   if x0[i+1]-x0[i] > 0.12/0.02:
			   health=False
			   print("窦性心率不齐！")
	if health==True:
		print("健康!")


i=0
k=0
x=[]
y_r=[]
y_t=0
print len(sys.argv)
if(len(sys.argv)!=3):
	pic=7
else:
	pic=sys.argv[2]

plt.ion()
number = sys.argv[1]
datapoint="patient" + str(number)+'_'+str(pic)+ ".txt" 
sshd = ssh_connect( * )
sftpd = sftp_open(sshd)
sftp_get(sftpd, str(number)+"/"+datapoint, 'doc'+'_'+datapoint)
sftp_close( sftpd )
ssh_close( sshd )

f=open('doc'+'_'+datapoint,'rb')
y=f.read()
max_i=len(y)
while i!=(max_i-1):
	if(y[i]!=','):
		y_t=y_t*10+ord(y[i])-48
	if(y[i]==','):
		y_r.append(y_t)
		y_t = 0
		k=k+1
		x.append(k)
	i=i+1
#print y_r
#print x
roads=y_r
Healthcare(roads)
plt.plot(x,y_r)
plt.pause(1000)
f.close()