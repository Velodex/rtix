# Copyright (c) 2025 Velodex Robotics, Inc and RTIX Developers.
# Licensed under Apache-2.0. http://www.apache.org/licenses/LICENSE-2.0

import logging
from typing import Dict
from dataclasses import dataclass

from google.protobuf.message import Message
from rtix.ipc.node import Node
from rtix.core.status import Status
from rtix.core.timer import Timer
from rtix.core.exception import RTIX_THROW_IF_NOT, getFullTraceback
from rtix.api import common_pb2


@dataclass
class InputConfig:
    """Configuration for input channels"""
    channel_id: str


@dataclass
class OutputConfig:
    """Configuration for output channels"""
    channel_id: str


@dataclass
class ProcessConfig:
    """
    Base process configuration
    
    Attributes:
        monitor_rate_s: The update rate when listening for an action
        loop_rate_s: The update rate when the loop is running
        action_channel_id: A channel for receiving actions from an orchestrator
        status_channel_id: A channel for returning status to an orchestrator
        input_channels: Input data channels (non-orchestrating)
        output_channels: Output data channels (non-orchestrating)
    """
    monitor_rate_s: float
    loop_rate_s: float
    action_channel_id: str
    status_channel_id: str
    input_channels: Dict[str, InputConfig]
    output_channels: Dict[str, OutputConfig]


class Process:
    """
    A class that responds to actions and publishes products and status.
    Protected methods are implemented in the derived class,  the start()
    function begins the process main loop.
    """

    def __init__(self, node: Node, config: ProcessConfig, action: Message):
        self._node = node
        self._config = config
        self._action = action
        self._running = False
        self._alive = False
        self._timer = Timer()

    def run(self):
        """This is the main process loop"""
        self._alive = True
        self._timer.start()
        while self._alive:
            try:
                loop_timer = Timer()

                # Process the inputs at the very beginning of the loop so that any
                # new actions can compute on the latest input information.
                self.stepOuter()

                # If available, handle a new action and reset the loop timer to
                # avoid counting the action processing time as an overrun.
                if self._hasNewAction():
                    self.handleAction(action=self._action)
                    loop_timer.start()

                # Run the step function and try to match the target process rate if
                # running, otherwise wait for a new action.
                if self._running:
                    self.stepInner()
                    self._matchTargetLoopRate(loop_timer.getElapsedS())
                else:
                    Timer.Sleep(self._config.monitor_rate_s)
            except Exception as e:
                logging.error(getFullTraceback())
                self.publishStatus(status=Status.FAILURE,
                                   msg="Exception {}".format(e))
                self._running = False

    def stop(self):
        """Ends the main loop"""
        self._alive = False

    def handleAction(self, action: Message):
        """
        Implement: This function is called after a new action is received.
        """
        raise NotImplementedError

    def stepOuter(self):
        """
        Implement: This function is called each iteration at the monitor_rate_s
        (if not running) or the loop_rate_s (if running).  This is where input
        information should be handled.
        """
        raise NotImplementedError

    def stepInner(self):
        """
        Implement: This function is called each iteration at the loop_rate_s (if
        running), and is not called if not running.  This is where runtime
        computation should be handled.
        """
        raise NotImplementedError

    def setRunning(self, running: bool):
        """Sets if the inner loop is running"""
        self._running = running

    def getProcessTimeS(self) -> float:
        """Returns the process time since the call of start()"""
        return self._timer.getElapsedS()

    def publishStatus(self, status: Status, msg: str):
        """Convenience method for publishing to the status channel"""
        outcome = common_pb2.Outcome(status=status.value, message=msg)
        if status == Status.FAILURE:
            logging.error(msg)
        else:
            logging.info(msg)
        self._node.publisher(self._config.status_channel_id).send(outcome)

    def publishOutput(self, key: str, msg: Message):
        """Convenience method for publishing to an output channel (by key)"""
        RTIX_THROW_IF_NOT(
            key in self._config.output_channels,
            "Process",
            f"Output key '{key}' not found in channels",
        )
        channel = self._config.output_channels[key]
        self._node.publisher(channel.channel_id).send(msg)

    def receiveInput(self, key: str, msg: Message) -> bool:
        """Convenience method for receiving from an input channel (by key)"""
        RTIX_THROW_IF_NOT(
            key in self._config.input_channels,
            "Process",
            f"Input key '{key}' not found in channels",
        )
        channel = self._config.input_channels[key]
        return self._node.subscriber(channel.channel_id).recv(msg, block=False)

    def flushInput(self, key: str):
        """Convenience method for flushing an input channel (by key)"""
        RTIX_THROW_IF_NOT(
            key in self._config.input_channels,
            "Process",
            f"Input key '{key}' not found in channels",
        )
        channel = self._config.input_channels[key]
        self._node.subscriber(channel.channel_id).flush()

    def _hasNewAction(self) -> bool:
        return self._node.subscriber(self._config.action_channel_id).recv(
            self._action, block=False)

    def _matchTargetLoopRate(self, elapsed_s: float):
        target_s = self._config.loop_rate_s
        if elapsed_s >= target_s:
            logging.debug(f"Task overrun of {elapsed_s - target_s} s")
        else:
            Timer.Sleep(target_s - elapsed_s)
