#!/usr/bin/env python3

import sys
import socket
import selectors
import types
import datetime

from utils.Parser import Parser

sel = selectors.DefaultSelector()


def accept_wrapper(sock):
    conn, addr = sock.accept()  # Should be ready to read
    print("Accepted connection from", addr)
    conn.setblocking(False)
    data = types.SimpleNamespace(addr = addr, inb = b"", outb = b"")
    events = selectors.EVENT_READ | selectors.EVENT_WRITE
    sel.register(conn, events, data = data)


def service_connection(key, mask):
    sock = key.fileobj
    data = key.data
    N_MESSAGES = 16
    CAN_MESSAGE_LEN = 1 + (41 + 2) * N_MESSAGES + 1 + 3

    try:
        if mask & selectors.EVENT_READ:
            recv_data = sock.recv(CAN_MESSAGE_LEN) # Should be ready to read
            if recv_data:
                data.outb += recv_data
            else:
                print("Closing connection to", data.addr)
                sel.unregister(sock)
                sock.close()

        if mask & selectors.EVENT_WRITE:
            if data.outb:
                ## CAN MESSAGE TO BE PARSED
                print(repr(data.outb))
                # b'[(1636363447.371728; 0C0#022AAC0000000000), (...; ...), ...]''
                message = repr(data.outb).replace("b", "").replace("'", "") # remove b''
                message = message.strip('][').split(',')
                print("SIZE:", len(message))

                file = "can.log"
                f = open(file, "a")

                # [(1636363447.371728; 0C0#022AAC0000000000), (...; ...), ...]
                for pair in message:
                    tuple = pair.strip(')(').split(';')
                    # ["ts", "id#msg"]

                    # get the timestamp
                    timestamp = float(tuple[0])

                    # id#msg
                    can = tuple[1].split("#")

                    # padding
                    #while(len(can[1]) < 16):
                        #can[1] = "00" + can[1]

                    f.write("(" + tuple[0] + ")\t" + can[0] + "#" + can[1] + "\n")

                    # id
                    id = int(can[0], 16)

                    # list with hex converted to int
                    n = 2
                    line = can[1]
                    print(line)
                    msg = [int(line[i:(i + n)], 16) for i in range(0, len(line), n)]

                    #print(timestamp, " - ", id, "#", msg)
                    p = Parser()
                    l_obj = p.parseMessage(timestamp, id, msg)
                    if(l_obj): # list not empty
                        for obj in l_obj:
                            print("Echoing ", obj.type, "informations:", data.addr)
                            dict = obj.get_dict()
                            for k, v in dict.items():
                                print("-", k, ": ", v)

                f.close()
                sent = sock.send(data.outb)  # Should be ready to write
                data.outb = data.outb[sent:]
    ## ERROR CATCHING
    except ConnectionResetError as e:
        print("Connection Reset Error:", e)
    except BrokenPipeError as e:
        print("Broken Pipe Error:", e)
    except OSError as e:
        print("OS Error:", e)

if len(sys.argv) != 3:
    print("Usage:", sys.argv[0], "<host> <port>")
    sys.exit(1)

host, port = sys.argv[1], int(sys.argv[2])

## write the address on file
#file = ""
#f = open(file, "a")
#f.truncate(0)
#f.write(address)
#f.close()

lsock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
lsock.bind((host, port))
lsock.listen()
print("Listening on", (host, port))
lsock.setblocking(False)
sel.register(lsock, selectors.EVENT_READ, data=None)

try:
    while True:
        events = sel.select(timeout = None)
        for key, mask in events:
            if key.data is None:
                accept_wrapper(key.fileobj)
            else:
                service_connection(key, mask)
except KeyboardInterrupt:
    print("Caught keyboard interrupt, exiting")
finally:
    sel.close()
