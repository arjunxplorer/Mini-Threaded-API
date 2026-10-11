# Mini-Threaded-API
A tiny C++ HTTP Server with basic threading to explore Concurrency and Parallelism.

# Goals
Single threaded GET that returns "Hello World"
Thread Pooling the requests
Background task for logs

# Why do we need threads?
When I transformed the server from taking only one request to being able to multiple request, then there came the problem: what if more than client is making a request at the same time?

Even more interesting: what if each request is delayed 5 seconds? Would that change the trajectory for every single client or just one client? 

The answer is, when we deal with more than one request then it is important that there are set of rules that each request should so that the other request could get an same access. It is happening because our server is single thread in the moment.

To make it happen, we need threads. To be specific, we need thread pooling.

# Using threads to complete the request in the same time
Before I starting adding 1000's of thread, I started with one thread. My initial code was completely inside the main function and to make the program run parallely I separate the client handling in the different function and called thread on that function. So the thread was initiated inside the while in the main function which means everytime a new client connection is established (accepted), then a new thread is fired. To prove that the request timing is improved, I added steady clock within the handleClient() to keep track of time for each request. This may not be the best way of measuring the overall time for each request but at the moment our program doesn't have to much overhead.


# Building thread pool
To understand the context of the current code, let's understand with an example. Suppose you run a resturant and you have one chef at the moment and resturant is not busy, but when the resturant is busy and as new orders are filling up you will hire a new chef in the moment. That will create a lot of overhead because you need to start calling new chef and they need to come over at the resturant and then start preparing the food. This introduces a lot of waiting time. But, what we have 10 chef or many number of chefs depending on the size of resturant and busyness ? Then, those chefs can sit inside the kitchen and whenever new order comes in then they are already in position to prepare the food. So, this will reduce that overhead.

This basically is thread pool in layman terms.

To introduce thread pool, we need queue (hold the clientsocket), mutex (serializes the access to the queue), condition_variable( wait if we don't have any more queues to proceed with the client request).

After this we can spin up number of required threads and inside each thread we will pass the worker function that will handle the queue and call handleClient function.
Inside worker(), we will acquire a unique lock and wait if there are no client socket available. In the main function inside while loop, we want to lock the queue so that we don't have the shared memory conflict. Once the client socket is added we will release the lock. Now, in worker we have unique lock because it protects the queue while checking whether it is empty or not. Also, it allows the conditional variable to release the mutex while the worker sleeps and acquire the lock when the worker is awake.

Once we know queue have a client socket, then we take the client socket out of the queue and release the lock and pass the clientSocket to the handleClient().