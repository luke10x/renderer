FROM emscripten/emsdk:3.1.25

WORKDIR /opt
RUN git clone https://github.com/recp/cglm.git \
  && mkdir /opt/cglm/build
WORKDIR /opt/cglm/build
RUN emcmake cmake .. \
  -DCMAKE_EXE_LINKER_FLAGS="-s STANDALONE_WASM" \
  -DCGLM_STATIC=ON
RUN emmake make install

