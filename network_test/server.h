#pragma once
#include <enet/enet.h>
#include <stdexcept>

struct Server {

  ENetAddress address;
  ENetHost* server;

  Server() {
    address.host = ENET_HOST_ANY;
    address.port = 1234;

    server = enet_host_create(&address /* the address to bind the server host to */,
      32      /* allow up to 32 clients and/or outgoing connections */,
      2      /* allow up to 2 channels to be used, 0 and 1 */,
      0      /* assume any amount of incoming bandwidth */,
      0      /* assume any amount of outgoing bandwidth */);

    if (server == NULL) { throw std::runtime_error("error with server"); }
  }


  void send_packet(ENetPeer* peer) {
    /* Create a reliable packet of size 7 containing "packet\0" */
    ENetPacket* packet = enet_packet_create("packet",
      strlen("packet") + 1,
      ENET_PACKET_FLAG_RELIABLE);
    /* Extend the packet so and append the string "foo", so it now */
    /* contains "packetfoo\0"                                      */
    //enet_packet_resize(packet, strlen ("packetfoo") + 1);
    //strcpy(&packet->data[strlen("packet")], "foo");
    /* Send the packet to the peer over channel id 0. */
    /* One could also broadcast the packet by         */
    /* enet_host_broadcast (host, 0, packet);         */
    enet_peer_send(peer, 0, packet);

  }


  void update() {
    ENetEvent event;
    /* Wait up to 300 milliseconds for an event. */
    while (enet_host_service(server, &event, 300) > 0) {
      switch (event.type) {
      case ENET_EVENT_TYPE_CONNECT:
        printf("A new client connected from %x:%u.\n",
          event.peer->address.host,
          event.peer->address.port);
        /* Store any relevant client information here. */
        //event.peer->data = "Client information";
        send_packet(event.peer);

        break;
      case ENET_EVENT_TYPE_RECEIVE:
        printf("A packet of length %u containing %s was received from %s on channel %u.\n",
          event.packet->dataLength,
          event.packet->data,
          event.peer->data,
          event.channelID);

        send_packet(event.peer);

        /* Clean up the packet now that we're done using it. */
        enet_packet_destroy(event.packet);

        break;

      case ENET_EVENT_TYPE_DISCONNECT:
        printf("server report: %s disconnected.\n", event.peer->data);
        /* Reset the peer's client information. */
        event.peer->data = NULL;
      }
    }

  }

  void cleanup() {
    enet_host_destroy(server);
  }

};