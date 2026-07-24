import socket
import threading
from queue import Queue

target_ip = "10.0.0.99" # The Linux machine
open_ports = []

def scan_port(port):
    try:
        sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        sock.settimeout(0.5)
        result = sock.connect_ex((target_ip, port))
        if result == 0:
            print(f"[+] Port {port} is OPEN")
            open_ports.append(port)
        sock.close()
    except:
        pass

def worker(q):
    while True:
        port = q.get()
        scan_port(port)
        q.task_done()

print(f"[*] Starting TCP scan on {target_ip}...")

port_queue = Queue()
for _ in range(50): # 50 threads
    thread = threading.Thread(target=worker, args=(port_queue,))
    thread.daemon = True
    thread.start()

for port in range(1, 1025): # Scan ports 1-1024
    port_queue.put(port)

port_queue.join()

print(f"\n[*] Scan complete! Open ports: {open_ports}")