def track_open_fds(log_file_path):
    open_fds = set()
    
    try:
        with open(log_file_path, 'r') as file:
            for line in file:
                line = line.strip()
                if not line:
                    continue
                
                parts = line.split()
                if len(parts) != 2:
                    continue
                
                cmd, fd = parts
                try:
                    fd = int(fd)
                except ValueError:
                    continue
                
                if cmd == 'add':
                    open_fds.add(fd)
                elif cmd == 'close':
                    if fd in open_fds:
                        open_fds.remove(fd)
    
    except FileNotFoundError:
        print(f"Error: File '{log_file_path}' not found")
        return []
    
    return sorted(open_fds)

if __name__ == "__main__":
    import sys
    
    if len(sys.argv) != 2:
        print("Usage: python fd_tracker.py <log_file_path>")
        sys.exit(1)
    
    result = track_open_fds(sys.argv[1])
    print("Open file descriptors:", result)
