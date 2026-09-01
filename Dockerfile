# .. (--network=host)
# docker build -t riskykvm . --platform linux/amd64
# docker run -v $PWD:/host -it riskykvm make -j -C /host/firmware/RISKYKVMBootloader
# ..

FROM ubuntu:24.04

# Add basic tooling
RUN apt-get update && apt install -y wget xz-utils make

# Download, extract and remove all but the 'RISC-V_Embedded_GCC' toolchain
RUN wget --progress=bar:force:noscroll -O- "http://file-oss.mounriver.com/tools/MRS_Toolchain_Linux_x64_V1.92.1.tar.xz" | tar --strip-components=1 -xJ -C "/opt" \
 && find /opt -mindepth 1 -maxdepth 1 -type d -not -name 'RISC-V_Embedded_GCC' -exec rm -r {} \;

ENV PATH="/opt/RISC-V_Embedded_GCC/bin:$PATH"
