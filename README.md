<h1 align="center">
	MiniChat :speech_balloon:
</h1>

<p align="center">
<b><i>A simple IRC server</i></b><br>
</p>

<p align="center">
	<img alt="GitHub code size in bytes" src="https://img.shields.io/github/languages/code-size/Bettercallous/MiniChat?color=red" />
	<img alt="Code language count" src="https://img.shields.io/github/languages/count/Bettercallous/MiniChat?color=green" />
	<img alt="GitHub top language" src="https://img.shields.io/github/languages/top/Bettercallous/MiniChat?color=blue" />
</p>

---

## 📜 Table of contents

  [**• Introduction**](#intro)

  [**• What is IRC ?**](#what)
  
  [**• Why use IRC ?**](#why)
  
  [**• How to use ?**](#how)
  
  [**• Server Commands**](#commands)
  
  [**• What is a Socket ?**](#socket)
  
  [**• How's a connection set between a client and a server ?**](#client-server-connection)
  
  [**• Resources**](#resources)

---

## 💡 Introduction : <a name="intro"></a>

<p align="left">
This test project is dedicated to exploring the complex workings of sockets and their interactions within the framework of an IRC chat/server• The primary goal is to replicate the functionality of an IRC server, offering a comprehensive understanding of its operation.
</p>

## What is IRC ? <a name="what"></a>
Internet Relay Chat (IRC) is an Internet application that was developed by Jakko Oikarinen in Finland• Chat is the most convenient immediate way to communicate with others via Internet• There are a number of topics called “channels” through which you can chat with many people all over the world• After joining channel, you can see what other people on this channel type on their keyboards. In that situation, everyone on this channel can see whatever you type on your keyboard. You can also hold individual conversations with someone. Channels get live on different servers around the world. Some servers have only a few channels, while others have many of them.

IRC follows client-server model. It means that both client and server software are required in order to use it. Many IT (Information Technology) clients are available for different kinds of computers, so whether you have a PC, Macintosh, or UNIX work-section, you will be able to use IRC. ~ [***source***](https://www.geeksforgeeks.org/internet-relay-chat-irc/)

<div align="center">
  <img src="/assets/IRC.png" alt="IRC Image" width="700" height="394" />
  <p><i>https://www.contus.com/blog/best-chat-and-instant-messaging-protocols/</i></p>
</div>

## Why use IRC ? <a name="why"></a>
IRC stands out for several reasons. Firstly, it's a protocol, not just a software, meaning its basic structure isn't tied to any specific program. This makes it reliable—your IRC channel won't disappear overnight if a particular platform shuts down or becomes restricted. Additionally, IRC is low-key and straightforward. You don't need to share personal details, and it's not cluttered with unnecessary features. While it may lack flashy avatars or intricate profiles, it excels at its core purpose:communication. For those who prefer simplicity and efficiency, the absence of unnecessary bells and whistles is actually a plus.

---

## How to use ? <a name="how"></a>

1. Clone the repository and run make

```bash
$> git clone git@github.com:Bettercallous/MiniChat.git
$> cd MiniChat
$> make
```

2. Run the server

```bash
$> ./minichat <port> <password>
```

- port: The port number on which the IRC server will be listening to for incoming IRC connections.
- password: The connection password. It will be needed by any IRC client that tries to connect to the server.

❗ **The server is compatible with ``irssi`` client, it may not work with other IRC clients.**

- Install [irssi](https://irssi.org/download/), then start it with simply typing irssi in the terminal.
- Connect to the server with the command `/connect <address> <port>` in the client.
- Now you must register in order to access the server. To do so, you'll be asked to provide the connection password, to set a nickname and a username as follows :
- `/quote PASS <serverpassword>`
- `/quote NICK <nickname>`
- `/quote USER <username> <hostname> <servername> <realname>`
> Don't stress about USER arguments they don't really matter.

> In Irssi, the /quote command is used to send raw IRC commands directly to the IRC server. This allows you to execute IRC commands that are not directly supported by Irssi's built-in commands or to send commands that require special formatting.

And that's it! Now you can join channels and send private messages in the server.

### Server Commands :  <a name="commands"></a>
> A command must be prefixed with slash / . Example: /JOIN random_channel

> A command can be either in uppercase or lowercase.

| Command | Description |
| :-----------: | :----------- |
| JOIN | Join a channel. If the channel specified does not exist, a new one will be created with the given name. |
| INVITE | Invite a user to a channel. |
| KICK | Eject a user from the channel. |
| TOPIC | Change or view the topic of the given channel. |
| MSG | Send private messages between users. |
| QUIT | Terminate a client’s connection to the server. |
| MODE | Set or remove options (or modes) to a given target. |

| Mode | Description |
| :-----------: | :----------- |
| i | Set/remove Invite-only channel. |
| o | Give/take channel operator privilege. |
| t | Set/remove the restrictions of the TOPIC command to channel operators. |
| k | Set/remove the channel key (password). |
| l | Set/remove the user limit to channel. |

---

## What is is a Socket ?  <a name="socket"></a>
- A socket is an end-point of communication between 2 devices (or processes).
- For 2 computers to send and receive a message over a network, they need to have a socket at both ends.
- A socket is identified by an IP address concatenated with a port number. example ( 146.86.5.20:1625 ).
- Sockets are commonly used for client and server interaction.

Here is an analogy:

Say you want to communicate/chat with your friend, who lives not at your address. For that to happen, you have to establish a "communication channel". Say, you want to do this communication using telephones. You know that there is a network of telephone lines in the city that is extended to every house.

Now, there is a telephone socket in your house, and one in your friends house. For the communication to take place, you and your friend have to connect to the network by plugging your phone to the socket, at the both end of the communication. Sockets in programming is the same, conceptually, as the telephone sockets.

In programming, you have two processes (running programs) that want to communicate with each other. For that, they have to establish a communication link between themselves. Again, there is a network available, they just need to connect to this network using some sort of sockets. Unix sockets are one such socket that provides this connectivity/pluggability to the network. So, in each of the two programs, you will have some piece of code that does the job of connecting to the network through sockets.

The rest are details.

## How's a connection set between a client and a server ?  <a name="client-server-connection"></a>

**1. Setting up the Server:**


The server creates a socket using the socket() function, specifying the address family (IPv4 or IPv6), socket type (typically SOCK_STREAM for TCP or SOCK_DGRAM for UDP), and protocol (usually 0 for default).
Next, the server binds the socket to a specific IP address and port using the bind() function. The server then listens for incoming connections using the listen() function, specifying the maximum number of queued connections. Once listening, the server enters a loop where it waits for incoming connection requests.


**2. Setting up the Client:**


The client creates a socket similar to the server using the socket() function. It then establishes a connection to the server by calling the connect() function, providing the server's IP address and port number.


**3. Handshake (TCP):**


For TCP connections, after the client initiates the connection, a handshake occurs between the client and server to establish the connection. This involves a series of messages exchanged between the client and server to ensure both parties are ready to communicate. Once the handshake is complete, the connection is established, and both client and server can send and receive data.

**4. Data Transmission:**


Once the connection is established, both client and server can send and receive data using the established socket connection.
Data is typically sent in chunks or packets. The send() function is used to send data from the client to the server, and the recv() function is used to receive data on the server side.
Conversely, the send() function is used on the server side to send data to the client, and the recv() function is used on the client side to receive data. 

**5. Closing the Connection:**


When the communication is complete, either the client or the server can initiate the closure of the connection. To close the connection, the client calls the close() function on its socket, or the server calls close() on the socket associated with the client's connection. This initiates a graceful closure, where any remaining data in transit is sent and acknowledged before the connection is fully closed.

<br>

<div align="center">
  <img src="/assets/socketworkings.png" alt="IRC Image" width="280" height="354" />
  <p><i>https://www.ibm.com/docs/en/i/7.3?topic=programming-how-sockets-work</i></p>
</div>

---

## 📖 Resources :  <a name="resources"></a>

- [Lewis Van Winkle - Hands On Network Programming with C](https://sourceforge.net/projects/booksforihucs/files/Lewis%20Van%20Winkle%20-%20Hands-On%20Network%20Programming%20with%20C%20-%20Learn%20socket%20programming%20in%20C%20and%20write%20secure%20and%20optimized%20network%20code%20%28true%20pdf%29-Packt%20%282020%29.pdf/download)
- [Beej’s Guide to Network Programming](https://beej.us/guide/bgnet/pdf/bgnet_a4_c_1.pdf)
- [Modern IRC Client Protocol](https://modern.ircdocs.horse/#privmsg-message) 
- [How Sockets Work](https://www.ibm.com/docs/en/i/7.3?topic=programming-how-sockets-work)
- [RPLs](https://www.alien.net.au/irc/irc2numerics.html)
- [RFC](https://datatracker.ietf.org/doc/html/rfc1459)

---
