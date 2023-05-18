# Use the official Bazel image
FROM gcr.io/bazel-public/bazel:latest

LABEL name="brainybuddy"

USER root

# Install required dependencies
RUN apt-get update && apt-get install -y --no-install-recommends \
        build-essential \
        libboost-dev \
        libboost-filesystem-dev \
        libboost-system-dev \
        libboost-thread-dev \
        libasio-dev \
        libcurl4-openssl-dev \
        libssl-dev \
        curl \
        gnupg \
        openjdk-11-jdk \
        bash \
        zip \
        unzip \
        python3 \
        git \
        patch \
    && rm -rf /var/lib/apt/lists/*

# Set the working directory
WORKDIR /app

# Copy the project files to the working directory
COPY . /app

# Set up your API tokens as environment variables
ENV DISCORD_BOT_TOKEN=$DISCORD_BOT_TOKEN
ENV OPENAI_API_KEY=$OPENAI_API_KEY

# Set the entrypoint
ENTRYPOINT ["bazel", "run", "//:my_bot"]
