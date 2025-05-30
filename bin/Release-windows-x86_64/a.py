import socket
import struct

SERVER_IP = "0.0.0.0"
SERVER_PORT = 12345
BUFFER_SIZE = 4096

players = {}  # key: name, value: dict with fields
clients = set()

def unpack_player(data: bytes):
    offset = 0

    # name
    if len(data) < offset + 1:
        raise ValueError("Incomplete packet for name length")
    name_len = data[offset]
    offset += 1

    if len(data) < offset + name_len:
        raise ValueError("Incomplete packet for name")
    name = data[offset:offset+name_len].decode("utf-8")
    offset += name_len

    # x, y floats
    if len(data) < offset + 8:
        raise ValueError("Incomplete packet for floats")
    x, y = struct.unpack_from("<ff", data, offset)
    offset += 8

    # currentblock_name
    if len(data) < offset + 1:
        raise ValueError("Incomplete packet for currentblock_name length")
    cbn_len = data[offset]
    offset += 1

    if len(data) < offset + cbn_len:
        raise ValueError("Incomplete packet for currentblock_name")
    currentblock_name = data[offset:offset+cbn_len].decode("utf-8")
    offset += cbn_len

    # chat_message
    if len(data) < offset + 1:
        raise ValueError("Incomplete packet for chat_message length")
    chat_len = data[offset]
    offset += 1

    if len(data) < offset + chat_len:
        raise ValueError("Incomplete packet for chat_message")
    chat_message = data[offset:offset+chat_len].decode("utf-8")
    offset += chat_len

    # changedblocks_count (uint16 little-endian)
    if len(data) < offset + 2:
        raise ValueError("Incomplete packet for changedblocks_count")
    changedblocks_count = struct.unpack_from("<H", data, offset)[0]
    offset += 2

    changedblocks = []
    for _ in range(changedblocks_count):
        if len(data) < offset + 12:
            raise ValueError("Incomplete packet for block packet")
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

    if len(name_bytes) > 255 or len(cbn_bytes) > 255 or len(chat_bytes) > 255:
        raise ValueError("String too long for packing")

    parts = []
    parts.append(struct.pack("B", len(name_bytes)))
    parts.append(name_bytes)
    parts.append(struct.pack("<ff", player["x"], player["y"]))
    parts.append(struct.pack("B", len(cbn_bytes)))
    parts.append(cbn_bytes)
    parts.append(struct.pack("B", len(chat_bytes)))
    parts.append(chat_bytes)

    # Pack changedblocks count
    parts.append(struct.pack("<H", len(changedblocks)))

    # Pack each block
    for block in changedblocks:
        parts.append(struct.pack("<iii", block["m_xpos"], block["m_ypos"], block["m_color"]))

    return b"".join(parts)


def pack_all_players(players: dict) -> bytes:
    parts = []
    for p in players.values():
        parts.append(pack_player(p))
    return b"".join(parts)


def main():
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    sock.bind((SERVER_IP, SERVER_PORT))
    print(f"UDP server listening on {SERVER_IP}:{SERVER_PORT}")

    while True:
        try:
            data, addr = sock.recvfrom(BUFFER_SIZE)
            clients.add(addr)

            try:
                player, consumed = unpack_player(data)
                print(f"Received player from {addr}: {player}")  # <--- added print here
            except Exception as e:
                print(f"Failed to unpack data from {addr}: {e}")
                continue

            # Remove player if position is (-555, -555)
            if player["x"] == -555.0 and player["y"] == -555.0:
                if player["name"] in players:
                    print(f"Removing player {player['name']} due to (-555, -555) position")
                    del players[player["name"]]
            else:
                players[player["name"]] = player

            # Broadcast all players to all clients
            packet = pack_all_players(players)
            for client_addr in clients:
                sock.sendto(packet, client_addr)

        except KeyboardInterrupt:
            print("\nServer stopped.")
            break


if __name__ == "__main__":
    main()
