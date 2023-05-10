# Use an official Ubuntu base image
FROM ubuntu:latest

# Update the package list and install required dependencies
RUN apt-get update && \
    apt-get install -y build-essential libboost-all-dev libasio-dev libcurl4-openssl-dev curl gnupg && \
    apt-get clean && \
    rm -rf /var/lib/apt/lists/*

# Install Bazel
RUN echo "deb [arch=amd64] https://storage.googleapis.com/bazel-apt stable jdk1.8" | tee /etc/apt/sources.list.d/bazel.list && \
    curl https://bazel.build/bazel-release.pub.gpg | apt-key add - && \
    apt-get update && \
    apt-get install -y bazel && \
    apt-get clean && \
    rm -rf /var/lib/apt/lists/*

# Set the working directory
WORKDIR /app

# Copy the project files to the working directory
COPY . /app

# Set up your API tokens as environment variables
CMD DISCORD_BOT_TOKEN=$DISCORD_BOT_TOKEN OPENAI_API_KEY=$OPENAI_API_KEY ./bazel-bin/my_bot

# Expose the required port for the bot (if needed)
# EXPOSE <port>

# Start the bot
CMD ["bazel", "--batch", "run", "//:my_bot"]
