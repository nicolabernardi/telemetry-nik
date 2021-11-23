import socket

HOST = '127.0.0.1'
PORT = 8000

with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
    s.bind((HOST, PORT))
    s.listen()
    while True:
        conn, addr = s.accept()
        with conn:
            print('Connected by ', addr)
            while True:
                data = conn.recv(673)
                print('Received: ', data.decode('utf-8'))
                print('\n\n\n\n\n\n\n\n')
                if not data or len(data) == 0:
                    break
                conn.sendall(data)
