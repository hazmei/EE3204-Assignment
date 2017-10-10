#!/usr/bin/python
# https://www.tutorialspoint.com/python/python_networking.htm
import socket

serversock = socket.socket()
host = socket.gethostname()
port = 8080
serversock.bind((host, port))

message = str.encode('hello world!')

print(message)
print(message.decode())

serversock.listen(5)
while True:
    c, addr = serversock.accept()
    print('Connection from {}'.format(addr))
    c.send(message)
    c.close()
