#pragma once
#include <enet/enet.h>
#include <stdexcept>

struct Client {
  ENetHost* client;

  Client() {
    client = enet_host_create(NULL /* create a client host */,
      1 /* only allow 1 outgoing connection */,
      2 /* allow up 2 channels to be used, 0 and 1 */,
      0 /* assume any amount of incoming bandwidth */,
      0 /* assume any amount of outgoing bandwidth */);
    if (client == NULL) {
      throw std::runtime_error("error with client");
    }
  }

  ENetAddress address;
  ENetPeer* peer;

  void connect() {
    /* Connect to localhost:1234. */
    enet_address_set_host(&address, "localhost");
    address.port = 1234;

    /* Initiate the connection, allocating the two channels 0 and 1. */
    peer = enet_host_connect(client, &address, 2, 0);

    if (peer == NULL) { throw std::runtime_error("error with client"); }

    ///* Wait up to 5 seconds for the connection attempt to succeed. */
    //if (enet_host_service (client, & event, 5000) > 0 &&
    //  event.type == ENET_EVENT_TYPE_CONNECT)
    //{
    //  puts ("Connection to localhost:1234 succeeded.");
    //}
    //else
    //{
    //  /* Either the 5 seconds are up or a disconnect event was */
    //  /* received. Reset the peer in the event the 5 seconds   */
    //  /* had run out without any significant event.            */
    //  enet_peer_reset (peer);
    //  puts ("Connection to localhost:1234 failed.");
    //}

  }

  void send_packet() {
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

    /* One could just use enet_host_service() instead. */
    //enet_host_flush(host);
    send_count++;
    printf("send from client count: %i\n", send_count);


  }


  int send_count{};
  int num_sends = 10;
  bool update() {

    ENetEvent event;
    /* Wait up to 300 milliseconds for an event. */
    while (enet_host_service(client, &event, 0) > 0) {
      switch (event.type) {
      case ENET_EVENT_TYPE_CONNECT:
        printf("connected to %x:%u.\n",
          event.peer->address.host,
          event.peer->address.port);
        /* Store any relevant client information here. */
        //event.peer->data = "Client information";
        send_packet();

        break;
      case ENET_EVENT_TYPE_RECEIVE:
        printf("A packet of length %u containing %s was received from %s on channel %u.\n",
          event.packet->dataLength,
          event.packet->data,
          event.peer->data,
          event.channelID);
        /* Clean up the packet now that we're done using it. */
        enet_packet_destroy(event.packet);

        send_packet();

        break;

      case ENET_EVENT_TYPE_DISCONNECT:
        printf("client report: %s disconnected.\n", event.peer->data);
        /* Reset the peer's client information. */
        event.peer->data = NULL;
      }
    }


    if (send_count > num_sends)
      return false;
    else
      return true;
  }

  void cleanup() {
    enet_host_destroy(client);
  }


};