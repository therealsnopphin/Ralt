import socket
import struct
import threading

SERVER_IP = "0.0.0.0"
SERVER_PORT = 12345

players = {}  # key: name, value: dict with fields
clients = set()
clients_lock = threading.Lock()

def recv_all(sock, n):
    """Receive exactly n bytes or return None if connection closed."""
    data = b''
    while len(data) < n:
        packet = sock.recv(n - len(data))
        if not packet:
            return None
        data += packet
    return data

def unpack_player(data: bytes):
    offset = 0

    name_len = data[offset]
    offset += 1
    name = data[offset:offset + name_len].decode("utf-8")
    offset += name_len

    x, y = struct.unpack_from("<ff", data, offset)
    offset += 8

    cbn_len = data[offset]
    offset += 1
    currentblock_name = data[offset:offset + cbn_len].decode("utf-8")
    offset += cbn_len

    chat_len = data[offset]
    offset += 1
    chat_message = data[offset:offset + chat_len].decode("utf-8")
    offset += chat_len

    changedblocks_count = struct.unpack_from("<H", data, offset)[0]
    offset += 2

    changedblocks = []
    for _ in range(changedblocks_count):
        xpos, ypos, color = struct.unpack_from("<iii", data, offset)
        offset += 12
        changedblocks.append({"m_xpos": xpos, "m_ypos": ypos, "m_color": color})

    player = {
        "name": name,
        "x": x,
        "y": y,
        "currentblock_name": currentblock_name,
        "chat_message": chat_message,
        "m_changedblocks": changedblocks,
    }

    return player, offset

def pack_player(player: dict) -> bytes:
    name_bytes = player["name"].encode("utf-8")
    cbn_bytes = player["currentblock_name"].encode("utf-8")
    chat_bytes = player["chat_message"].encode("utf-8")
    changedblocks = player.get("m_changedblocks", [])

    parts = []
    parts.append(struct.pack("B", len(name_bytes)))
    parts.append(name_bytes)
    parts.append(struct.pack("<ff", player["x"], player["y"]))
    parts.append(struct.pack("B", len(cbn_bytes)))
    parts.append(cbn_bytes)
    parts.append(struct.pack("B", len(chat_bytes)))
    parts.append(chat_bytes)
    parts.append(struct.pack("<H", len(changedblocks)))

    for block in changedblocks:
        parts.append(struct.pack("<iii", block["m_xpos"], block["m_ypos"], block["m_color"]))

    return b''.join(parts)

def pack_all_players(players: dict) -> bytes:
    parts = []
    for p in players.values():
        parts.append(pack_player(p))
    return b''.join(parts)

def handle_client(conn, addr):
    print(f"[+] Connection from {addr}")
    with conn:
        with clients_lock:
            clients.add(conn)

        try:
            while True:
                length_prefix = recv_all(conn, 4)
                if not length_prefix:
                    break
                data_length = struct.unpack("<I", length_prefix)[0]

                data = recv_all(conn, data_length)
                if not data:
                    break

                try:
                    player, _ = unpack_player(data)
                except Exception as e:
                    print(f"[!] Failed to unpack from {addr}: {e}")
                    continue

                if player["x"] == -555.0 and player["y"] == -555.0:
                    if player["name"] in players:
                        print(f"[x] Removing player {player['name']}")
                        del players[player["name"]]
                else:
                    players[player["name"]] = player

                packet = pack_all_players(players)
                packet_len = struct.pack("<I", len(packet))

                with clients_lock:
                    for client in list(clients):
                        try:
                            client.sendall(packet_len + packet)
                        except:
                            clients.remove(client)

        except Exception as e:
            print(f"[!] Client {addr} disconnected with error: {e}")

        with clients_lock:
            clients.discard(conn)
    print(f"[-] Disconnected from {addr}")

def main():
    server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    server.bind((SERVER_IP, SERVER_PORT))
    server.listen()
    print(f"TCP server listening on {SERVER_IP}:{SERVER_PORT}")

    try:
        while True:
            conn, addr = server.accept()
            threading.Thread(target=handle_client, args=(conn, addr), daemon=True).start()
    except KeyboardInterrupt:
        print("\nServer shutting down.")
    finally:
        server.close()

if __name__ == "__main__":
    main()
