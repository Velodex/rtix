# RTIX: Runtime Interprocess Execution

RTIX is a fast and lightweight IPC and orchestration layer for robotics and embodied AI applications.

## Why RTIX?

Existing frameworks like ROS-2, while powerful and feature-rich, are large and often result in application source code that is difficult to onboard, develop, and maintain.  RTIX takes a minimalist approach to avoid the large dependency headache so developers can own their stack end-to-end.

Our core design principles with RTIX are:
- **Minimal dependencies**: There are only a few core dependencies.
  - Nng is a lightweight IPC in both C++ and Python.
  - Protobuf is the industry standard for message serialization and deserialization.
  - YAML is used for configuration of the data plane.
  - Spdlog is used for C++ logging.
- **Low latency IPC**: We use shared memory to make communication as fast as possible and mitigate the non-determinism that presents in networking based approaches.
- **Simple**: Our lightweight design makes installation, utilization, and versioning straightforward.  This improves maintainability.

## Basic Usage
The following shows basic usage in Python.  C++ follows a similar pattern.

```python
import yaml
from rtix.ipc.node import Node
from rtix.ipc.channel_map import ChannelMap

# Initialize the channel map.  This is where the connections are managed.
with open("channel-map.yaml") as file:
    yaml_dict = yaml.safe_load(file)
    channel_map = ChannelMap.LoadYaml(yaml_dict)

# A node contains multiple publishers and subscribers.  It is typically used as
# the IPC manager for a single process.
node = Node(config=channel_map.nodes["my_node"])

# Messages are Protobuf types.  You can use built-in Protobuf messages or
# create your own.  This is a placeholder for a real message type.
my_message = Message()

# The publisher send command is sync/blocking.  Returns true if
# the message was sent to the shared memory successfully.
node.publisher("my_message").send(my_message)

# The subscriber recv command is by default sync/blocking for the
# length of the timeout.  If block=False then it will return
# immediately.  Returns true if data was pulled from the shared
# memory, false if not.
node.subscriber("my_message").recv(my_message, block=True)
```

The snippet above is for reference only.  In practice, messages will be published and received in different processes or threads.  For real examples, see below.


## Installation
Method 3 is recommended for development and extensive use.

### Method 1: Pypi (Python only)
The simpliest way to try out RTIX is to install from pypi.
```bash
python3 -m pip install rtix
```
Note that the pip installation uses pre-generated Protobuf messages.  If you use a different version of Protobuf, you should install from source to regenerate the messages.

### Method 2: Source (C++ and Python)
We recommend using the Development with Docker method below.  Proceed if you are comfortable managing your dependency environment.

1. Clone the package.
2. Install the core package dependencies found in the [Dockerfile](./Dockerfile).  The Dockerfile shows dependency installation for Ubuntu using apt.  For Mac and Windows platforms, you will need to install packages using dependency management for those platforms.
3. Install the Python requirements.txt
```bash
python3 -m pip install -r requirements.txt
```
4. Create and build a CMake project.  Protobuf files are generated during the CMake configuration step.
```bash
mkdir build
cd build
cmake ..
make -j
make test
```

### Method 3: [Recommended] Development with Docker (C++ and Python)
While it is not required, we strongly recommend using Docker to manage multiple processes.  Docker is a containerized dependency management system.  We provide a Dockerfile with the necessary dependencies for RTIX development.

1. Install [Docker Desktop](https://www.docker.com/products/docker-desktop/).
2. Clone the package.
3. Build the Docker container.  This sets up a containerized environment for development.
```bash
docker build -f Dockerfile -t rtix-dev .
```
4. Run the Docker container.
```bash
docker run --rm -it -v .:/rtix rtix-dev
```
5. In the container, setup a CMake project, build and test.  Protobuf files are generated during the CMake configuration step.
```bash
mkdir build
cd build
cmake ..
make -j
make test
```
6. If you wish to generate a coverage report, rebuild using the `TEST_COVERAGE=ON`.  Note that this turns off optimizations, so to use in an application, you'll want to delete the `build` folder and rebuild without the coverage flag on.
```bash
cmake -DTEST_COVERAGE=ON ..
make -j && make test

# Commands for both XML and HTML are provided
make coverage-xml
make coverage-html
```
7. Voila!  You're ready to develop, test, and run any of the examples.

## Performance
TODO

## Examples
1. [Simple Python Publish and Subscriber](./examples/python_pub_sub/README.md)

## License
Licensed permissively under Apache-2.0.

## Contributing
TODO
