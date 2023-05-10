# BrainyBuddy

This BrainyBuddy bot is designed to answer questions using OpenAI's GPT-3.5 Turbo. It listens for messages in Discord, determines if the message is a question, processes the question, and sends it to the OpenAI API to get an answer using GPT-3.5 Turbo. The bot then returns the answer to the user as a reply in the Discord channel the message was sent.

![BrainyBuddy bot example](https://i.postimg.cc/nr4b6NLF/Zrzut-ekranu-2023-05-09-080749.png)

## Features

- Monitors Discord channels for messages that are questions
+ Determines if a message is a question based on punctuation, keywords, and text embeddings
* Processes and sends the question to OpenAI's GPT-3 Turbo API
+ Returns the answer to the user as a replay in the Discord channel
- Utilizes Bazel for building and managing dependencies

## Prerequisites

- Bazel build system
* C++11 compatible compiler
+ Boost library
- curl library
* ASIO library
* Docker (optional)

## How to build and run

### Without Docker
1. Install the required dependencies on your system:
For Ubuntu and similar systems, you can run:
 ```
 sudo apt-get install build-essential libboost-all-dev libasio-dev libcurl4-openssl-dev
 ```
2. Then, follow the official Bazel installation guide for Ubuntu to install Bazel: https://docs.bazel.build/versions/main/install-ubuntu.html
3. Clone this repository and navigate to the project folder.

4. Set up your API tokens for both Discord and OpenAI's GPT-3 as environment variables:
```
export DISCORD_BOT_TOKEN=your-discord-bot-token
export OPENAI_API_KEY=your-openai-api-key
```
<sub>Replace discord-bot-token and openai-api-key with your actual Discord bot token and OpenAI API key, respectively.<sub>

5. To build and run the bot, execute the following command:
```
bazel --batch run //:my_bot
```
<sub>This command runs Bazel in batch mode to avoid potential memory leaks associated with the Bazel server.<sub>

6. The bot will connect to Discord and start monitoring messages.

### With Docker
1. Install Docker on your system.

2. Clone this repository and navigate to the project folder.

3. Create a .env file in the project folder with the following content:
```
DISCORD_BOT_TOKEN=your-discord-bot-token
OPENAI_API_KEY=your-openai-api-key
 ```
<sub>Replace discord-bot-token and openai-api-key with your actual Discord bot token and OpenAI API key, respectively.<sub>
 
3. Build the Docker image:
 ```
docker build -t brainybuddy:latest .
 ```
4. Run the Docker container:
 ```
docker run -d --name brainybuddy-container --env-file .env brainybuddy:latest
 ```
5. The bot will connect to Discord and start monitoring messages.
 
## Configuration
To use this bot, you need to set up a Discord bot account and provide your API tokens for both Discord and OpenAI's GPT-3. Update the environment variables with the appropriate tokens and any other necessary settings.

## Contributing

Feel free to submit issues or conntact me for any improvements or fixes you'd like to see in this project. I'am always looking for ways to enhance the bot and make it more useful.

