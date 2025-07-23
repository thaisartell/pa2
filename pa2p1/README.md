## Names:
Richard Sartell, Kip Schetnan

## x500:
sarte016, schet076

## Division of task across members
Richard: queue structure/creation, queue functions, queue initialization/assignment, some sync stuff (creating mutexes, some locking), worker_thread function.

Kip: Implemented the main function and fixed functions above to correct. Added some error handling as well. Added some debugging lines for our personal use.

## Lab machine used for testing purpose.
Richard: Linux Mint 21.3 OS
Kip: login01

## Design specifications: Mention your project design in details and mention any challenges faced.
Challenges: Figuring out the infinite loop in main to read the file in 1024 byte chunks. Took some time to understand how to implement.

## Document and explain any usage of AI tools.
Richard: ChatGPT gave me the idea a circular buffer would be easiest to use. Helped me troubleshoot why my worker_thread wait-loop was non-satisfactory.

Kip: ChaptGPT helped me correctly print results in the correct format needed.

## Anything the TA should know about.
Richard: N/A.
Kip: N/A.