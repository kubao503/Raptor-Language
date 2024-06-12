FROM ubuntu:latest

WORKDIR /interpreter

RUN apt-get update \
    && apt-get install -y g++ \
    && apt-get install -y curl

COPY conanfile.txt .
COPY CMakeLists.txt .
COPY example.rp .
COPY src/ src/
COPY tests/ tests/
COPY setup.sh .

RUN chmod +x setup.sh

CMD ["./setup.sh"]
