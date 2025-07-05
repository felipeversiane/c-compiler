# Imagem base com GCC
FROM gcc:12.2.0-bullseye AS builder

# Instalar make
RUN apt-get update && apt-get install -y \
    make \
    && rm -rf /var/lib/apt/lists/*

# Configurar diretório de trabalho
WORKDIR /app

# Copiar arquivos necessários
COPY Makefile ./
COPY include/ include/
COPY src/ src/
COPY examples/ examples/

# Compilar o projeto
RUN make 