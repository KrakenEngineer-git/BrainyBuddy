# BrainyBuddy

This BrainyBuddy bot is designed to answer questions using OpenAI's GPT-3 Turbo. It listens for messages in Discord that start with !question, processes the question, and sends it to the OpenAI API to get an answer using GPT-3 Turbo. The bot then returns the answer to the user as a replay in the Discord channel the message was send.

![BrainyBuddy bot example](https://i.postimg.cc/XJFxHjPz/Zrzut-ekranu-2023-05-08-080310.png)


## Features

- Monitors Discord channels for messages starting with !question
* Processes and sends the question to OpenAI's GPT-3 Turbo API
+ Returns the answer to the user as a replay in the Discord channel
- Utilizes Bazel for building and managing dependencies

## Prerequisites

- Bazel build system
* C++11 compatible compiler
+ Boost library
- curl library
* ASIO library

## How to build and run

1. Install the required dependencies on your system:
For Ubuntu and similar systems, you can run:
 ```
 sudo apt-get install build-essential libboost-all-dev libasio-dev libcurl4-openssl-dev
 ```

2. Clone this repository and navigate to the project folder.

3. Set up your API tokens for both Discord and OpenAI's GPT-3 as environment variables:
```
export DISCORD_BOT_TOKEN=<your-discord-bot-token>
export OPENAI_API_KEY=<your-openai-api-key>
```
<sub>Replace <your-discord-bot-token> and <your-openai-api-key> with your actual Discord bot token and OpenAI API key, respectively.<sub>

4. To build and run the bot, execute the following command:
```
bazel --batch run //:my_bot
```
<sub>This command runs Bazel in batch mode to avoid potential memory leaks associated with the Bazel server.<sub>

5. The bot will connect to Discord and start monitoring messages.

## Configuration
To use this bot, you need to set up a Discord bot account and provide your API tokens for both Discord and OpenAI's GPT-3. Update the environment variables with the appropriate tokens and any other necessary settings.

## Contributing

Feel free to submit issues or conntact me for any improvements or fixes you'd like to see in this project. I'am always looking for ways to enhance the bot and make it more useful.

