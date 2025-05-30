import socket
import struct

SERVER_IP = "0.0.0.0"
SERVER_PORT = 12345
BUFFER_SIZE = 4096  # Increased for bigger packets

players = {}  # key: name, value: dict with player fields including blocks
clients = set()

def unpack_blockpacket(data: bytes, offset: int):
    """
    Unpack a BlockPacket from data at offset.
    Returns (blockpacket_dict, new_offset)
    """
    if len(data) < offset + 8:
        raise ValueError("Incomplete blockpacket")
    m_worldcoordinate, color = struct.unpack_from("<ii", data, offset)
    offset += 8
    return {"m_worldcoordinate": m_worldcoordinate, "color": color}, offset

def unpack_player(data: bytes):
    """
    Unpack a single GamePacket from data bytes.
    Return (player_dict, bytes_consumed)
    """
    offset = 0

    # Name
    if len(data) < offset + 1:
        raise ValueError("Incomplete packet for name length")
    name_len = data[offset]
    offset += 1

    if len(data) < offset + name_len:
        raise ValueError("Incomplete packet for name")
    name = data[offset:offset + name_len].decode("utf-8")
    offset += name_len

    # floats x, y
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
    currentblock_name = data[offset:offset + cbn_len].decode("utf-8")
    offset += cbn_len

    # chat_message
    if len(data) < offset + 1:
        raise ValueError("Incomplete packet for chat_message length")
    chat_len = data[offset]
    offset += 1

    if len(data) < offset + chat_len:
        raise ValueError("Incomplete packet for chat_message")
    chat_message = data[offset:offset + chat_len].decode("utf-8")
    offset += chat_len

    # m_changedblocks vector
    if len(data) < offset + 2:
        raise ValueError("Incomplete packet for m_changedblocks count")
    (num_blocks,) = struct.unpack_from("<H", data, offset)  # uint16 count
    offset += 2

    m_changedblocks = []
    for _ in range(num_blocks):
        block, offset = unpack_blockpacket(data, offset)
        m_changedblocks.append(block)

    player = {
        "name": name,
        "x": x,
        "y": y,
        "currentblock_name": currentblock_name,
        "chat_message": chat_message,
        "m_changedblocks": m_changedblocks,
    }
    return player, offset

def pack_blockpacket(block: dict) -> bytes:
    """
    Pack a BlockPacket struct into bytes.
    """
    return struct.pack("<ii", block["m_worldcoordinate"], block["color"])

def pack_player(player: dict) -> bytes:
    """
    Pack a single GamePacket struct into bytes.
    """
    name_bytes = player["name"].encode("utf-8")
    cbn_bytes = player["currentblock_name"].encode("utf-8")
    chat_bytes = player["chat_message"].encode("utf-8")

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

    # m_changedblocks vector length (uint16)
    num_blocks = len(player.get("m_changedblocks", []))
    parts.append(struct.pack("<H", num_blocks))

    for block in player.get("m_changedblocks", []):
        parts.append(pack_blockpacket(block))

    return b"".join(parts)

def pack_all_players(players: dict) -> bytes:
    """
    Pack all players into one binary message:
    uint16 number_of_players
    repeated player structs
    """
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