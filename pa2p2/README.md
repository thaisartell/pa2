1. Team members.
Richard Sartell, Kip Schetnan

2. x500.
sarte016, schet076

3. Division of task across members.
Richard: client + README
Kip: server + testing

4. Lab machine used for testing purpose.
Richard: Dell G15 Laptop - Linux Mint 22
Kip: Lab machine: login01

5. Design specifications: Mention your project design in details and mention any challenges faced.
Richard: The client side of the project continually copies data to and processes data from char chunk[1024]. It largely follows the code outline provided in the original assignment comments.
Kip: TCP socket server that listens for connections from multiple clients. When client connects, server creates new thread to handle client. Careful mutex lock + deadlock avoidance

6. Document and explain any usage of AI tools.
Richard: Used as a double-check to correct any mistakes I've made with req_message_t, resp_message_t variables. Also had it help me improve error-driven teardowns.
Kip: Used ChatGPT to troubleshoot run script FileNotFound errors.

7. Anything the TA should know about.
Richard: N/A
Kip: N/A.