####################################################################################
# Copyright 2018 ON Semiconductor.  All rights reserved.
#
# This software and/or documentation is licensed by ON Semiconductor under limited
# terms and conditions.  The terms and conditions pertaining to the software and/or
# documentation are available at http://www.onsemi.com/site/pdf/ONSEMI_T&C.pdf
# ("ON Semiconductor Standard Terms and Conditions of Sale, Section 8 Software").
# Do not use this software and/or documentation unless you have carefully read and
# you agree to the limited terms and conditions.  By using this software and/or
# documentation, you agree to the limited terms and conditions.
#
####################################################################################
import argparse
import random
import socket
import string
import sys

from xmlrpc.server import SimpleXMLRPCServer
from xmlrpc.server import SimpleXMLRPCRequestHandler
from xmlrpc.client import ServerProxy

class RequestHandler(SimpleXMLRPCRequestHandler):
    """Restrict XML RPC requests to a particular path."""
    rpc_paths = ('/RPC2',)

class TokenService():
    """The services provided by the token server."""
    def __init__(self):
        self.__tokens = {}
    
    def restart(self):
        """Restarts the service."""
        self.__tokens = {}
        return (0, 'Service restarted successfully')
        
    def status(self):
        """Returns status message."""
        unused = 0
        for value in self.__tokens.values():
            if value is None:
                unused += 1
        return (0, 'Token server is running and has {} tokens with {} unused'.format(
            len(self.__tokens), unused))
        
    def new_token(self):
        """Generates a new unique key and returns it."""
        token = ''.join(random.choices(string.ascii_lowercase + string.digits, k=16))
        if token in self.__tokens:
            return (1, 'Duplicate token generated')
        else:
            self.__tokens[token] = None
            return (0, token)
        
    def set_token(self, token, value):
        """Sets the value of a token."""
        if token in self.__tokens and self.__tokens[token] is None:
            self.__tokens[token] = value
            return (0, 'Success')
        else:
            return (1, 'Invalid token')
        
    def get_token(self, token):
        """Returns the value of a token."""
        value = self.__tokens.get(token, None)
        if value is None:
            return (1, 'Invalid token')
        else:
            return (0, value)

    def delete_token(self, token):
        """Deletes the named token."""
        if token in self.__tokens:
            del self.__tokens[token]
        return (0, 'Success')

def do_start_server(args):
    # Create and start the server (always done on the current host)
    with SimpleXMLRPCServer((socket.gethostname(), args.port), requestHandler=RequestHandler, logRequests=False) as server:
        # Register the token service object with the XML RPC server
        server.register_instance(TokenService())
        # Run the server main loop
        print('Listening on port {}...'.format(args.port))
        server.serve_forever()

def get_server_url(args):
    """Returns the token server url."""
    return 'http://{}:{}/RPC2'.format(args.hostname, args.port)

def do_restart(args):
    """Restart the service."""
    with ServerProxy(get_server_url(args), verbose=args.debug) as proxy:
        status, message = proxy.restart()
        if status:
            print(message, file=sys.stderr)
            sys.exit(status)
        # Print server message
        print(message)

def do_status(args):
    """Prints the server status."""
    with ServerProxy(get_server_url(args), verbose=args.debug) as proxy:
        status, message = proxy.status()
        if status:
            print(message, file=sys.stderr)
            sys.exit(status)
        # Print server status message
        print(message)

def do_new_token(args):
    """Request new token from the server."""
    with ServerProxy(get_server_url(args), verbose=args.debug) as proxy:
        status, message = proxy.new_token()
        if status:
            print(message, file=sys.stderr)
            sys.exit(status)
        # Print token on stdout
        print(message)
    
def do_set_token(args):
    """Set token value on the the server."""
    with ServerProxy(get_server_url(args), verbose=args.debug) as proxy:
        status, message = proxy.set_token(args.name, args.value)
        if status:
            print(message, file=sys.stderr)
            sys.exit(status)
    
def do_get_token(args):
    """Request token value from the server (and delete it)."""
    with ServerProxy(get_server_url(args), verbose=args.debug) as proxy:
        status, message = proxy.get_token(args.name)
        if status:
            print(message, file=sys.stderr)
            sys.exit(status)  
        # Print token value on stdout            
        print(message)
        # Tokens are one time use only so free the memory
        status, message = proxy.delete_token(args.name)
        if status:
            print(message, file=sys.stderr)
            sys.exit(status)  
    
# Create the argument parser
parser = argparse.ArgumentParser(prog='token_server',
    description='Token server that manages socket connections for the Firmware Test Access Server/Client pairs')
parser.add_argument('-d', '--debug', default=False, action='store_true', help='Enable verbose debug')
parser.add_argument('-n', '--hostname', default='gbbr01-jenkins', help='Server host name')
parser.add_argument('-p', '--port', type=int, default=9080, help='Server port number')
subparsers = parser.add_subparsers()
# Define start sub-command
start_parser = subparsers.add_parser('start', help='Start the token server')
start_parser.set_defaults(func=do_start_server)
# Define restart sub-command
restart_parser = subparsers.add_parser('restart', help='Restart the service')
restart_parser.set_defaults(func=do_restart)
# Define status sub-command
status_parser = subparsers.add_parser('status', help='Print server status')
status_parser.set_defaults(func=do_status)
# Define new sub-command
new_parser = subparsers.add_parser('new', help='Create a new token name')
new_parser.set_defaults(func=do_new_token)
# Define set sub-command
set_parser = subparsers.add_parser('set', help='Set the token to the string value')
set_parser.add_argument('name', help='Token name string')
set_parser.add_argument('value', help='Token value string')
set_parser.set_defaults(func=do_set_token)
# Define get sub-command
get_parser = subparsers.add_parser('get', help='Get the token value (and delete server token)')
get_parser.add_argument('name', help='Token name string')
get_parser.set_defaults(func=do_get_token)

# Parse the command line arguments and call the sub-command handler
args = parser.parse_args()
args.func(args)
