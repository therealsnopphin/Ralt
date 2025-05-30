#pragma once

struct BlockPacket {
    int m_xpos;
    int m_ypos;
    int m_color;
};

struct GamePacket {
    std::string name;
    float x;
    float y;
    std::string currentblock_name;
    std::string chat_message;
    std::vector<BlockPacket> m_changedblocks;
};

class SimpleUDPSocket {
public:
    using ReceiveCallback = std::function<void(std::vector<GamePacket>&& players)>;

    SimpleUDPSocket(const char* serverIp, uint16_t serverPort)
        : m_serverPort(serverPort), m_serverIp(serverIp), m_running(false), m_socket(INVALID_SOCKET) {
    }

    ~SimpleUDPSocket() {
        sendPlayerPosition({ "", -555, -555 });
        stop();
    }

    bool start() {
        WSADATA wsaData;
        if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
            std::cerr << "WSAStartup failed\n";
            return false;
        }

        m_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
        if (m_socket == INVALID_SOCKET) {
            std::cerr << "UDP Socket creation failed: " << WSAGetLastError() << "\n";
            WSACleanup();
            return false;
        }

        m_serverAddr.sin_family = AF_INET;
        m_serverAddr.sin_port = htons(m_serverPort);
        inet_pton(AF_INET, m_serverIp, &m_serverAddr.sin_addr);

        m_running = true;
        m_receiveThread = std::thread(&SimpleUDPSocket::receiveLoop, this);

        return true;
    }

    void stop() {
        m_running = false;
        if (m_receiveThread.joinable()) {
            m_receiveThread.join();
        }
        if (m_socket != INVALID_SOCKET) {
            closesocket(m_socket);
            m_socket = INVALID_SOCKET;
        }
        WSACleanup();
    }

    void sendPlayerPosition(const GamePacket& player) {
        std::vector<char> buffer;

        buffer.push_back(static_cast<uint8_t>(player.name.size()));
        buffer.insert(buffer.end(), player.name.begin(), player.name.end());

        appendToBuffer(buffer, player.x);
        appendToBuffer(buffer, player.y);

        buffer.push_back(static_cast<uint8_t>(player.currentblock_name.size()));
        buffer.insert(buffer.end(), player.currentblock_name.begin(), player.currentblock_name.end());

        buffer.push_back(static_cast<uint8_t>(player.chat_message.size()));
        buffer.insert(buffer.end(), player.chat_message.begin(), player.chat_message.end());

        uint16_t count = static_cast<uint16_t>(player.m_changedblocks.size());
        buffer.push_back(static_cast<char>(count & 0xFF));
        buffer.push_back(static_cast<char>((count >> 8) & 0xFF));

        for (const auto& block : player.m_changedblocks) {
            appendToBuffer(buffer, block.m_xpos);
            appendToBuffer(buffer, block.m_ypos);
            appendToBuffer(buffer, block.m_color);
        }

        sendto(m_socket, buffer.data(), (int)buffer.size(), 0, (sockaddr*)&m_serverAddr, sizeof(m_serverAddr));
    }

    void setReceiveCallback(ReceiveCallback cb) {
        m_receiveCallback = std::move(cb);
    }

private:
    template<typename T>
    void appendToBuffer(std::vector<char>& buf, const T& val) {
        const char* p = reinterpret_cast<const char*>(&val);
        buf.insert(buf.end(), p, p + sizeof(T));
    }

    bool readFromBuffer(const char* data, size_t data_len, size_t& offset, void* out, size_t size) {
        if (offset + size > data_len) return false;
        std::memcpy(out, data + offset, size);
        offset += size;
        return true;
    }

    void receiveLoop() {
        constexpr int BUFFER_SIZE = 8192;

        while (m_running) {
            char recvBuffer[BUFFER_SIZE];
            sockaddr_in fromAddr;
            int fromLen = sizeof(fromAddr);

            int recvLen = recvfrom(m_socket, recvBuffer, BUFFER_SIZE, 0, (sockaddr*)&fromAddr, &fromLen);
            if (recvLen <= 0) continue;

            const char* packetData = recvBuffer;
            size_t offset = 0;
            size_t packetSize = recvLen;

            std::vector<GamePacket> players;

            while (offset < packetSize) {
                if (offset + 1 > packetSize) break;
                uint8_t name_len = (uint8_t)packetData[offset++];
                if (offset + name_len + 8 > packetSize) break;
                std::string name(packetData + offset, name_len); offset += name_len;

                float x, y;
                if (!readFromBuffer(packetData, packetSize, offset, &x, sizeof(float))) break;
                if (!readFromBuffer(packetData, packetSize, offset, &y, sizeof(float))) break;

                if (offset + 1 > packetSize) break;
                uint8_t cbn_len = (uint8_t)packetData[offset++];
                if (offset + cbn_len > packetSize) break;
                std::string cbn(packetData + offset, cbn_len); offset += cbn_len;

                if (offset + 1 > packetSize) break;
                uint8_t chat_len = (uint8_t)packetData[offset++];
                if (offset + chat_len > packetSize) break;
                std::string chat(packetData + offset, chat_len); offset += chat_len;

                if (offset + 2 > packetSize) break;
                uint16_t count = (uint8_t)packetData[offset] | ((uint8_t)packetData[offset + 1] << 8);
                offset += 2;

                std::vector<BlockPacket> blocks;
                for (int i = 0; i < count && offset + 12 <= packetSize; ++i) {
                    int xpos, ypos, color;
                    if (!readFromBuffer(packetData, packetSize, offset, &xpos, sizeof(int))) break;
                    if (!readFromBuffer(packetData, packetSize, offset, &ypos, sizeof(int))) break;
                    if (!readFromBuffer(packetData, packetSize, offset, &color, sizeof(int))) break;
                    blocks.push_back({ xpos, ypos, color });
                }

                players.push_back({ name, x, y, cbn, chat, std::move(blocks) });
            }

            if (m_receiveCallback && !players.empty()) {
                m_receiveCallback(std::move(players));
            }
        }
    }

    SOCKET m_socket;
    sockaddr_in m_serverAddr{};
    uint16_t m_serverPort;
    const char* m_serverIp;

    std::thread m_receiveThread;
    std::atomic<bool> m_running;
    ReceiveCallback m_receiveCallback;
};
