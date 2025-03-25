FROM osrf/ros:noetic-desktop

# Add ubuntu user with same UID and GID as your host system, if it doesn't already exist
# Since Ubuntu 24.04, a non-root user is created by default with the name vscode and UID=1000
# ARG USERNAME=ubuntu
# ARG USER_UID=1000
# ARG USER_GID=$USER_UID
# RUN if ! id -u $USER_UID >/dev/null 2>&1; then \
#         groupadd --gid $USER_GID $USERNAME && \
#         useradd -s /bin/bash --uid $USER_UID --gid $USER_GID -m $USERNAME; \
#     fi
# # Add sudo support for the non-root user
# RUN apt-get update && \
#     apt-get install -y sudo && \
#     echo "$USERNAME ALL=(root) NOPASSWD:ALL" > /etc/sudoers.d/$USERNAME && \
#     chmod 0440 /etc/sudoers.d/$USERNAME

# # Switch from root to user
# USER $USERNAME

# Add user to video group to allow access to webcam
# RUN sudo usermod --append --groups video $USERNAME

# Update all packages
RUN sudo apt update && sudo apt upgrade -y

# Install Git
RUN sudo apt install -y git

# Rosdep update
RUN rosdep update

# Source the ROS setup file
RUN echo "source /opt/ros/${ROS_DISTRO}/setup.bash" >> ~/.bashrc

# install catkin anc others specified by ocs2
RUN apt update \
    && apt install -y libglpk-dev \
    python3-catkin-tools \
    ros-noetic-pybind11-catkin \
    python3-catkin-tools \
    doxygen \
    doxygen-latex \
    liburdfdom-dev \
    liboctomap-dev \
    libassimp-dev \
    && rm -rf /var/lib/apt/lists/*

# install rqt plot tool
RUN apt-get update \
    && apt-get install -y ros-noetic-rqt-multiplot \
    checkinstall \
    wget \
    && rm -rf /var/lib/apt/lists/*

# Clone pinocchio, hpp-fcl and raisim
WORKDIR /workspace/src
RUN git clone --recurse-submodules https://github.com/leggedrobotics/pinocchio.git \
    && git clone --recurse-submodules https://github.com/leggedrobotics/hpp-fcl.git \
    && git clone https://github.com/leggedrobotics/ocs2_robotic_assets.git \
    && git clone --depth 1 https://github.com/raisimTech/raisimLib.git -b v1.1.01

#install grid map msgs, needed from raisim
RUN sudo apt update \
&& apt-get install -y ros-noetic-grid-map-msgs \
&& rm -rf /var/lib/apt/lists/*

# install raisim
# Add the following lines to your ~/.bashrc file
RUN echo 'export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$WORKSPACE/raisim/linux/lib' >> ~/.bashrc \
    && echo 'export PYTHONPATH=$PYTHONPATH:$WORKSPACE/raisim/linux/lib' >> ~/.bashrc
RUN cd /workspace/src/raisimLib \
    && mkdir build \
    && cd build \
    && cmake .. -DRAISIM_EXAMPLE=ON -DRAISIM_PY=ON -DPYTHON_EXECUTABLE=$(which python3) \
    && make -j4 && sudo checkinstall

# install mpc-net related
# RUN cd /tmp \
#     && wget https://github.com/microsoft/onnxruntime/releases/download/v1.7.0/onnxruntime-linux-x64-1.7.0.tgz \
#     && tar xf onnxruntime-linux-x64-1.7.0.tgz \
#     && mkdir -p ~/.local/bin ~/.local/include/onnxruntime ~/.local/lib ~/.local/share/cmake/onnxruntime \
#     && rsync -a /tmp/onnxruntime-linux-x64-1.7.0/include/ ~/.local/include/onnxruntime \
#     && rsync -a /tmp/onnxruntime-linux-x64-1.7.0/lib/ ~/.local/lib \
#     && rsync -a /workspace/src/ocs2/ocs2_mpcnet/ocs2_mpcnet_core/misc/onnxruntime/cmake/ ~/.local/share/cmake/onnxruntime

# Set up catkin workspace
WORKDIR /workspace
RUN catkin init \
    && catkin config --extend /opt/ros/${ROS_DISTRO} \
    && catkin config -DCMAKE_BUILD_TYPE=RelWithDebInfo

# build catkin workspace
WORKDIR /workspace
RUN catkin build

# source catkin pkgs
RUN echo "source /workspace/devel/setup.bash" >> ~/.bashrc

# return to root user for other images
USER root
################################
## ADD ANY CUSTOM SETUP BELOW ##
################################
