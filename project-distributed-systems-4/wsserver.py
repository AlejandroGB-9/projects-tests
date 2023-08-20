from spyne import Application, ServiceBase, String, Unicode, rpc
from spyne.protocol.soap import Soap11
from spyne.server.wsgi import WsgiApplication

class ProxyService(ServiceBase):
    @rpc(String, String, String, _returns=String)
    def register(ctx, name, alias, date):
        return "s< REGISTER OK"
    
    @rpc(String, _returns=String)
    def unregister(ctx, alias):
        return "s< UNREGISTER OK"
    
    @rpc(String, String, String, _returns=String)
    def connect(ctx, alias, port):
        return "s< CONNECT"+alias+"OK"
    
    @rpc(String, _returns=String)
    def disconnect(ctx, alias):
        return "s< DISCONNECT"+alias+"OK"
    
    @rpc(String, _returns=String)
    def connected_users(ctx, alias):
        return "s< CONNECTEDUSERS OK"

    @rpc(String, String, String, _returns=String)
    def send(ctx, alias, alias2, message):
        return "s< SEND"+alias2+message+"OK"
    
application = Application([ProxyService], tns="http://localhost:8000", in_protocol=Soap11(validator='lxml'), out_protocol=Soap11())
application = WsgiApplication(application)

if __name__ == '__main__':
    import logging
    from wsgiref.simple_server import make_server
    logging.basicConfig(level=logging.DEBUG)
    logging.getLogger('spyne.protocol.xml').setLevel(logging.DEBUG)
    logging.info("listening to http://localhost:8000")
    logging.info("wsdl is at: http://localhost:8000/?wsdl")
    server = make_server('localhost', 8000, application)
    server.serve_forever()