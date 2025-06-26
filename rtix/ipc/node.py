# Copyright (c) 2025 Velodex Robotics, Inc and RTIX Developers.
# Licensed under Apache-2.0. http://www.apache.org/licenses/LICENSE-2.0

from __future__ import annotations
import logging
from typing import Dict, List, Any
from dataclasses import dataclass
import pynng as nng

from google.protobuf.message import Message
import rtix.types.common_pb2 as common_pb2

from rtix.core.timer import getTimestampNs

# Config dictionary constants
NODES_KEY = "nodes"
SUBCRIBERS_KEY = "subscribers"
PUBLISHERS_KEY = "publishers"
CHANNEL_KEY = "channel_id"
TIMEOUT_KEY = "timeout_ms"
MSG_PREFIX = b"m"


def packMessage(msg: Message) -> str:
    """Packs the protobuf message into a binary data packet with metadata"""
    packet = common_pb2.Packet()
    packet.payload.Pack(msg)
    packet.metadata.timestamp_ns = getTimestampNs()
    data = packet.SerializeToString()
    return MSG_PREFIX + data


def unpackMessage(data: str, metadata: common_pb2.Metadata, msg: Message):
    """Unpacks a binary data packet into a protobuf message and metadata"""
    packet = common_pb2.Packet()
    metadata.Clear()
    msg.Clear()
    n = len(MSG_PREFIX)
    if len(data) > n:
        packet.ParseFromString(data[n:])
        metadata.CopyFrom(packet.metadata)
        packet.payload.Unpack(msg)


class Publisher:
    """Primary interface for publishing messages to a channel"""

    @dataclass
    class Config:
        channel_id: str

        @staticmethod
        def LoadYaml(yaml_dict: Dict[str, Any]) -> Publisher.Config:
            """Creates the config object loaded from YAML"""
            return Publisher.Config(channel_id=yaml_dict[CHANNEL_KEY])

    def __init__(self, config: Config):
        """Initializes the publisher from config"""
        self._channel_id = config.channel_id
        self._address = "ipc:///tmp/" + config.channel_id + ".ipc"
        self._socket = nng.Pub0(listen=self._address)
        logging.info("Started publisher '{}' at {}".format(
            self._channel_id, self._address))

    def __del__(self):
        """Cleans up the socket"""
        self._socket.close()

    def send(self, msg: Message) -> bool:
        """Send the protobuf message (blocking), returns True on success"""
        try:
            data = packMessage(msg)
            self._socket.send(data)
            logging.debug("Publisher '{}' sent data".format(self._channel_id))
            return True
        except:
            logging.error("Publisher '{}' send failed".format(
                self._channel_id))
        return False


class Subscriber:
    """Primary interface for subscribing to messages from a channel"""

    @dataclass
    class Config:
        channel_id: str
        timeout_ms: int

        @staticmethod
        def LoadYaml(yaml_dict: Dict[str, Any]) -> Subscriber.Config:
            """Creates the config object loaded from YAML"""
            return Subscriber.Config(
                channel_id=yaml_dict[CHANNEL_KEY],
                timeout_ms=yaml_dict[TIMEOUT_KEY],
            )

    def __init__(self, config: Config):
        """Initializes the subscriber from config"""
        self._channel_id = config.channel_id
        self._timeout_ms = config.timeout_ms
        self._address = "ipc:///tmp/" + config.channel_id + ".ipc"
        # If block_on_dial is unset, an error will be logged even if the dial
        # is completed asynchronously through retries.
        self._socket = nng.Sub0(dial=self._address,
                                recv_timeout=config.timeout_ms,
                                block_on_dial=False)
        self._socket.subscribe(b"")  # everything
        self._socket.recv_buffer_size = 1  # important to ensure latest
        logging.info("Started subscriber '{}' at {}".format(
            self._channel_id, self._address))

    def __del__(self):
        """Cleans up the socket"""
        self._socket.close()

    def flush(self):
        """Flush the buffer"""
        try:
            self._socket.recv(block=False)
        except:
            pass

    def recv(self,
             msg: Message,
             metadata: common_pb2.Metadata = common_pb2.Metadata(),
             block: bool = True) -> bool:
        """Receive the protobuf message (blocking), returns True when received"""
        try:
            data = self._socket.recv(block=block)
            unpackMessage(data, metadata, msg)
            logging.debug(f"Subscriber '{self._channel_id}' received data")
            return True
        except nng.Timeout:
            return False
        except Exception as e:
            if block:
                logging.error(
                    f"Subscriber '{self._channel_id}' recv failed with: {e}")
        return False


class Node:
    """Primary interface to manage multiple pub/sub within a single process"""

    @dataclass
    class Config:
        publishers: List[Publisher.Config]
        subscribers: List[Subscriber.Config]

        @staticmethod
        def LoadYaml(yaml_dict: Dict[str, Any]) -> Node.Config:
            """Creates the config object loaded from YAML"""
            return Node.Config(
                publishers=[
                    Publisher.Config.LoadYaml(yaml_dict=pub)
                    for pub in yaml_dict[PUBLISHERS_KEY]
                ],
                subscribers=[
                    Subscriber.Config.LoadYaml(yaml_dict=pub)
                    for pub in yaml_dict[SUBCRIBERS_KEY]
                ],
            )

    def __init__(self, config: Node.Config):
        """Initializes the node from config"""
        self._pubs = {}
        for pub in config.publishers:
            self._pubs[pub.channel_id] = Publisher(config=pub)

        # Subscribers are spun up second because the channel needs to exist
        self._subs = {}
        for sub in config.subscribers:
            self._subs[sub.channel_id] = Subscriber(config=sub)

    def publisher(self, channel_id: str) -> Publisher:
        """Returns a publisher by key"""
        return self._pubs[channel_id]

    def subscriber(self, channel_id: str) -> Subscriber:
        """Returns a subscriber by key"""
        return self._subs[channel_id]
