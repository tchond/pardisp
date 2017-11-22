Some comments:

- To compile the code simply use "make"
- Part of the code includes the CH implementation from KIT. This code does not come with our own implementation (which is not yet complete). 
- In the code, the shortest path variant is not working 100%. The middle segment is retrieved, but the first and last segments not. I am really sorry, but if that's what you need, you will have to implement that yourself for now.
- Along with the code I send you a sample dataset (oldenburg.gr) along with three sample partitions. I used METIS to partition the network (http://glaros.dtc.umn.edu/gkhome/metis/metis/overview). You can use it to partition other networks and use them as input.
- To run just use

$ ./run.exec -f [ROAD_NETWORK] -p [PARTITION]

I apologize for the messy code. Unfortunately I haven't worked on this project for a long time and I never really bothered to clean the code properly. In any case, if you have any questions or you need something more, please let me know. I will do my best to assist you. 