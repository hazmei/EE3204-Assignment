#!/usr/bin/python
#https://www.tutorialspoint.com/python/python_networking.htm
import socket

clientsock = socket.socket()
host = socket.gethostname()
port = 8080

clientsock.connect((host, port))
print(clientsock.recv(1024))
clientsock.close
