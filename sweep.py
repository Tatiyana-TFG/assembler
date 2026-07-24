import subprocess
import concurrent.futures

base_ip = "10.0.0."

def check_host(ip_suffix):
    result = subprocess.run(
        # CHANGED '-c' to '2' to survive the initial ARP drop
        ['ping', '-c', '2', '-W', '1000', f"{base_ip}{ip_suffix}"],
        stdout=subprocess.DEVNULL, 
        stderr=subprocess.DEVNULL  
    )
    
    if result.returncode == 0:
        return f"{base_ip}{ip_suffix}"
    return None

print("[*] Scanning network with 20 workers...")

with concurrent.futures.ThreadPoolExecutor(max_workers=20) as executor:
    results = executor.map(check_host, range(1, 255))

print("[*] Live hosts found:")
for ip in results:
    if ip is not None:
        print(f"[+] {ip}")