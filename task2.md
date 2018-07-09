# Round Robin Scheduling

### Graph Plotting Description

In order to complete the task of plotting average wait and turn around times for round robin scheduling I used a combination of items from part 1 as well as a python script. The exact steps I followed are:
1. Create a directory called `data/round_robin` that will hold all output files of generated data.
2. Redirect output from task 1 to instead point to a filename with the format `quantum_%d_dispatch_%d.txt` and use the `simgen` file provided by Dr.Zastre to generate tasks. Some examples are shown below:
```
./simgen 1000 8296 | ./rrsim --quantum 50 --dispatch 0
./simgen 1000 8296 | ./rrsim --quantum 100 --dispatch 0
./simgen 1000 8296 | ./rrsim --quantum 250 --dispatch 0
./simgen 1000 8296 | ./rrsim --quantum 500 --dispatch 0
./simgen 1000 8296 | ./rrsim --quantum 50 --dispatch 5
./simgen 1000 8296 | ./rrsim --quantum 100 --dispatch 5
.... and so on
```
3. We now have a directory full of files with names like `quantum_50_dispatch_0.txt` which can be parsed by our python script.
4. The python script first creates 2 maps, one for the average wait time and one for the average turn around time. Each key of the respective maps will correspond to the quantum level used for that test run, we can extract this information from the filename using regex as such `quantum_regex = re.compile("(.*)_dispatch")`. Similarly we can get the index for which the data point will exist within the array as the dispatch number divided by 5 `re.compile('dispatch_(.*).txt')/5`.
5. Using these 2 maps we can call our `plot_round_robin` function twice, each with the unique map.
6. Using the `matplotlib` library in python, the data can be plotted at each quantum level as follows `plt.plot(dispatch, data["quantum_50"], color='purple', marker='|', label="quantum_50")`, which will grab the array inside our data map that has the key `quantum_50` and plot that onto our graph.


### Additional Feature

In addition to the round robin scheduling algorithm, I have added the first come first serve algorithm to rrsim.c. Using this function, I was able to compare average wait and turnaround times between the two algorithms. The Graphs for first come first serve are contained in the files `graph_waiting_fcfs.pdf` and `graph_turnaround_fcfs.pdf`. We can see from the comparison in wait times between first come first serve and round robin that first come first serve has a lower average wait time. This is consistent with the fact that the average wait times for the round robin scheduling algorithm decrease as the available quantum goes up. As the quantum increases, it becomes more similar to first come first serve and if the quantum is higher than the max cpu requested by any task then we can gaurantee that round robin will act the same way as first come first serve because all tasks will be allowed to finish before getting interupted. Looking at the average turn around times we can see that once again the first come first serve algorithm has a lower average turn around time when compared to the round robin algorithm. Overall, I am surprised by the preformance comparisons between fcfs and round robin, however they are consistent with the changing in quantum levels as shown in the respective round robin graphs.