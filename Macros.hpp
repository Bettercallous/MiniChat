#ifndef MACROS_HPP
#define MACROS_HPP

#define OO1 ":irc.l9oroch 001 " + nickname + " :Welcome to the MiniChat Network, " + nickname + '\n'
#define OO2 ":irc.l9oroch 002 " + nickname + " :Your host is MiniChat, running version 4.5" + '\n'
#define OO3 ":irc.l9oroch 003 " + nickname + " :This server was created " + formatCreationTime() + '\n'
#define OO4 ":irc.l9oroch 004 " + nickname + " MiniChat MiniChat(enterprise)-2.3(12)-netty(5.4c)-proxy(0.9) MiniChat" + '\n'

#define JOIN_MESSAGE(nickname, channelName) (":" + nickname + " JOIN #" + channelName + "\n")
#define MODE_MESSAGE(channelName) (":irc.l9oroch MODE #" + channelName + " +nt\n")
#define NAMES_MESSAGE(nickname, channelName) (":irc.l9oroch 353 " + nickname + " = #" + channelName + " :@" + nickname + "\n")
#define NAMES_MESSAGE2(nickname, channelName) (":irc.l9oroch 353 " + nickname + " @ #" + channelName + " :")
#define END_OF_NAMES_MESSAGE(nickname, channelName) (":irc.l9oroch 366 " + nickname + " #" + channelName + " :End of /NAMES list.\n")
#define CHANNEL_MESSAGE(channelName, creationTimeMessage) (":irc.l9oroch 354 " + channelName + " " + creationTimeMessage + "\n")
#define TOPIC_MESSAGE(nickname, channelName, topic) (":irc.l9oroch 332 " + nickname + " #" + channelName + " :" + topic + " https://irc.com\n")
#define PRIVATE_MESSAGE(senderFd, recipient, message) (":" + nicknames[senderFd] + " PRIVMSG " + recipient + " :" + message + "\r\n")
#define ERROR_MESSAGE(senderFd, recipient) (":server.host NOTICE " + nicknames[senderFd] + " :Error: User '" + recipient + "' not found or offline\r\n")



#define BUFFER_SIZE 1024
#define RED "\033[31m"
#define GREEN "\033[32m"
#define CYAN "\033[36m"
#define RESET "\033[0m"

#define MINICHAT  "\n$$\\      $$\\ $$\\           $$\\  $$$$$$\\  $$\\                  $$\\     \n" \
                  "$$$\\    $$$ |\\__|          \\__|$$  __$$\\ $$ |                 $$ |    \n" \
                  "$$$$\\  $$$$ |$$\\ $$$$$$$\\  $$\\ $$ /  \\__|$$$$$$$\\   $$$$$$\\ $$$$$$\\   \n" \
                  "$$\\$$\\$$ $$ |$$ |$$  __$$\\ $$ |$$ |      $$  __$$\\  \\____$$\\_$$  _|  \n" \
                  "$$ \\$$$  $$ |$$ |$$ |  $$ |$$ |$$ |      $$ |  $$ | $$$$$$$ | $$ |    \n" \
                  "$$ |\\$  /$$ |$$ |$$ |  $$ |$$ |$$ |  $$\\ $$ |  $$ |$$  __$$ | $$ |$$\\ \n" \
                  "$$ | \\_/ $$ |$$ |$$ |  $$ |$$ |\\$$$$$$  |$$ |  $$ |\\$$$$$$$ | \\$$$$  |\n" \
                  "\\__|     \\__|\\__|\\__|  \\__|\\__| \\______/ \\__|  \\__| \\_______|  \\____/ \n\n"


#endif