import serial 
import math
import matplotlib.pyplot as plt
import matplotlib.animation as animation
import time 
import sys
import paramiko
import os


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

x=[]
y=[]
i=0
k=0
pic=0
t = serial.Serial('com7',9600)  
plt.ion()
plt.plot(x, y)
number = sys.argv[1]
while 1:
	datapoint="patient" + str(number)+'_'+str(pic)+ ".txt" 
	picture = "patient" + str(number)+'_'+str(pic)+ ".png" 
	f=open(datapoint,'wb')
	while 1:
		str1 = t.read()
		x.append(i)
		y.append(ord(str1))
		f.write(str(ord(str1)))
		f.write(',')
		i=i+1  

		if(i>=200):
			f.write('&')
			sshd = ssh_connect( * ) /servername
			sftpd = sftp_open(sshd)
			plt.clf()
			plt.plot(x, y)
			plt.pause(1)
			if(pic<10):
				plt.savefig(picture, format='png')
				try:
					sftp_put(sftpd, picture, str(number)+"/"+picture)
				except Exception, e:
					print 'ERROR: sftp_put - %s' % e
			f.close()
			try:
				sftp_put(sftpd, datapoint, str(number)+"/"+datapoint)
			except Exception, e:
				print 'ERROR: sftp_put - %s' % e
			sftp_close( sftpd )
			ssh_close( sshd )
			i = 0
			x = []
			y = []
			pic = pic + 1
			break
