
#include "ShooterMsgType.h"

sf::Packet &operator<<(sf::Packet &packet, ShooterMsgType type) {
    return packet << (sf::Uint16) type;
}

sf::Packet &operator>>(sf::Packet &packet, ShooterMsgType &type) {
    sf::Uint16 temp;
    packet >> temp;
    type = (ShooterMsgType) temp;
    return packet;
}