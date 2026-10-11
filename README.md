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
