import zeep
import socket

def register(client, name, alias, date):
    
    return client.service.register(name, alias, date)
    
def unregister(client, alias):
    
    return client.service.unregister(alias)
    
def connect(client, alias, port):
    
    return client.service.connect(alias, port)
    
def disconnect(client, alias):
    
    return client.service.disconnect(alias)
    
def send(client, alias, alias2, message):
    
    return client.service.send(alias, alias2, message)
    
def connected_users(client, alias):
    
    return client.service.connected_users(alias)

def main():
    hostname = socket.gethostname()
    ip_address = socket.gethostbyname(hostname)
    wsdl = 'http://'+ip_address+':8000/?wsdl'
    client = zeep.Client(wsdl=wsdl)
    