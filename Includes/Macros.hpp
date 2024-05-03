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
#define TOPIC_MESSAGE2(nicknamesender, channelname, topic) (":" + nicknamesender + " TOPIC #" + channelname + " :" + topic + "\n")

#define PRIVATE_MESSAGE(senderFd, recipient, message) (":" + nicknames[senderFd] + " PRIVMSG " + recipient + " :" + message + "\r\n")
#define ERROR_MESSAGE(senderFd, recipient) (":server.host NOTICE " + nicknames[senderFd] + " :Error: User '" + recipient + "' not found or offline\r\n")
#define INVITE_MESSAGE(senderFd, recipient, channelName) (":" + nicknames[senderFd] + " INVITE " + recipient + " :#" + channelName + "\r\n")
#define KICK_MESSAGE(nicknamesender, channelname, usertokick, reason) (":" + nicknamesender + " KICK #" + channelname + " " + usertokick + " :" + reason + "\n")
#define KICK_MESSAGE2(channelName, fd, userToKick, reason) (":" + channels[channelName].getNickname(fd) + " KICK #" + channelName + " " + userToKick + " :" + reason + "\n")

#define MODE_CHANGE_MESSAGE(channelname, mode, sender, receiver) \
    (mode == "+t" ? MODE_CHANGE_T(channelname, sender) : \
    (mode == "-t" ? MODE_CHANGE_MINUS_T(channelname, sender) : \
    (mode == "+o" ? MODE_CHANGE_O(channelname, sender, mode, receiver) : \
    (mode == "-o" ? MODE_CHANGE_MINUS_O(channelname, sender, mode, receiver) : \
    (mode == "+i" ? MODE_CHANGE_IK(channelname, sender, mode) : \
    (mode == "-i" ? MODE_CHANGE_IK(channelname, sender, mode) : \
    (mode == "+k" ? MODE_CHANGE_IK(channelname, sender, mode) : \
    (mode == "-k" ? MODE_CHANGE_IK(channelname, sender, mode) : \
    (mode == "+l" ? MODE_CHANGE_L(channelname, sender, mode) : \
    (mode == "-l" ? MODE_CHANGE_L(channelname, sender, mode) : ""))))))))))

#define MODE_CHANGE_T(channelname, sender) (MODE_CHANGE_MINUS_T(channelname, sender))
#define MODE_CHANGE_MINUS_T(channelname, sender) (":server.host MODE #" + channelname + " -t by " + sender + "\n")
#define MODE_CHANGE_O(channelname, sender, mode, receiver) (":server.host MODE #" + channelname + " " + mode + " by " + sender + " and set " + receiver + "as operator\n")
#define MODE_CHANGE_MINUS_O(channelname, sender, mode, receiver) (":server.host MODE #" + channelname + " " + mode + " by " + sender + " and unset " + receiver + "as operator\n")
#define MODE_CHANGE_IK(channelname, sender, mode) (":server.host MODE #" + channelname + " " + mode + " by " + sender + "\n")
#define MODE_CHANGE_L(channelname, sender, mode) (":server.host MODE #" + channelname + " " + mode + " by " + sender + "\n")

#define ERROR_MESSAGE2(nick) (":server.host NOTICE " + nick + what)
#define ERROR_MESSAGE3(sender, channelName, userToKick) (":" + sender + " PRIVMSG #" + channelName + " :Error: the user : " + userToKick + " is not found or offline.\r\n")
#define ERROR_MESSAGE4(sender, channelName, userToKick) (":" + sender + " PRIVMSG #" + channelName + " :Error1: You are not authorized to execute this command " + userToKick + "\r\n")
#define ERROR_MESSAGE5() (":" + channels[channelName].getNickname(fd) + " PRIVMSG #" + channelName + " :Error: You are not authorized to execute this command " + "\r\n")
#define ERROR_MESSAGE6(sender, channelName) (":" + sender + " PRIVMSG #" + channelName + " :Error2: You are not authorized to execute this command\r\n")
#define ERROR_MESSAGE7(channelName, fd) (":" + channels[channelName].getNickname(fd) + " PRIVMSG #" + channelName + " :Error: You are not authorized to execute this command\r\n")

#define CONGRATS_MSG(guessed) (":server.host PRIVMSG #:Congratulations ,you have guessed the number : " + guessed + " correctly\n")
#define GUESS_AGAIN(random) (":server.host PRIVMSG #:Sorry ,the correct one is : " + random + "\n")
#define GUESS_ERROR() (":server.host PRIVMSG # :Error: Invalid range or guess\n")
#define MODE_SET_MESSAGE(channelName, mode, fd, nick) (":server.host MODE #" + channelName + " " + mode + " by " + channels[channelName].getNickname(fd) + " and set " + nick + "as operator\n")
#define MODE_UNSET_MESSAGE(channelName, mode, fd, nick) (":server.host MODE #" + channelName + " " + mode + " by " + channels[channelName].getNickname(fd) + " and unset " + nick + "as operator\n")
#define TOPIC_CHANGE_MESSAGE(channelName, mode, fd) (":server.host MODE #" + channelName + " " + mode + " by " + channels[channelName].getNickname(fd) + "\n")

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

extern int opperatorfd;
extern int issettop;
extern int isinvited;
extern int itHasPass;
extern int channelLimit;
extern int limitchannelforincrement;
extern int abaaba;

#endif