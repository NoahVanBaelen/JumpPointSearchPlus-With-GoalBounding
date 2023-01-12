# JumpPointSearchPlus-With-GoalBounding 

1-Description of the topic

Jump point search with goal-bounding is one form of jump point search that takes advantage of goal bounding to speed up the algorithm.
The Jump Point Search (JPS) algorithm is a faster derivative of A*.
Since it does not need to check and update its “open and closed” lists and also it can be used for different graph types.
JPS comes with the drawback that the cost must be the same for all horizontal and vertical edges, as the cost must be the same for all diagonal edges.

The algorithm works by first doing a horizontal scan of the current node.

Beginning with the current node and proceeding to its left until it encounters a forced neighbour (a wall) or the goal node. 
If it encounters either of these conditions, then it is put into the “open and closed” list and then continuing with scanning on the right side.

Next move, is a vertical scan this is equal to the horizontal scan but instead now the action is up and down.

After this, we carry out diagonal scans of our current node.
This works as follows: for each of the top right, top left, bottom right, and bottom left corners, we shift one step at a cost of 1.5 until we find a forced neighbour around us or we find ourselves at the end of the grid at each of these diagonal scans.
We also perform a horizontal and vertical scan for each node.

After all these scans, we take the cheapest node from the open list for our new jump point.

Goal-bounding works by looking at the 4 edges of the start node, we uses a technique known as Dijkstra's four-way floodfill algorithm but we take in account from which edge we came and pass on that information to our neighbour nodes.
We then use this information to create 4 bounding boxes on each side of the start node, then we do look at which bounding box our goal node is in and use that one to skip nodes outside our boundary. 
The disadvantage of having a bounding box is that our map must be fixed, since you cannot add or remove walls while it is running.

https://github.com/Neleab/JumpPointSearchPlus-With-GoalBounding/blob/main/Picture1.png?raw=true

Fig. 1 Shows the start node as well as all his edges. 
Each neighboring node stores which edge it comes from.
Since there is a wall next to 2 there will be no bounding box created from this side.

https://github.com/Neleab/JumpPointSearchPlus-With-GoalBounding/blob/main/Picture2.png?raw=true

Fig. 2 Shows a bounding box made up of all tiles that start at edge number 4

https://github.com/Neleab/JumpPointSearchPlus-With-GoalBounding/blob/main/Picture3.png?raw=true

Fig. 3 Shows a bounding box made up of all tiles that start at edge number 3.

https://github.com/Neleab/JumpPointSearchPlus-With-GoalBounding/blob/main/Picture4.png?raw=true

Fig. 4 Shows a bounding box made up of all tiles that start at edge number 1.

2-Design/implementation

For my implementation, I used a gridgraph. To my opinion the gridgraph could show the jump point search process better than an ordinary node graph or navemesh.

Before I start the path between the start node and the goal node, I first create a bounding box using a technique known as Dijkstra's four-way floodfill algorithm,
taking in count from which edge we came and we feed that information to our neighbour nodes.

As a maximum of four bounding boxes are created, we look at which bounding box our goal node is in and use that one.

Then we go to the jump point search part, where we start with a start node and put it in the open list and closed list. 
The open list contains all the nodes that have to be scanned and a closed list of all found jump points. Then from the current node we do a horizontal scan, one step to the right each time increasing the cost by 1 until we get to the end of the grid, where we find a wall or goal node. 
If we find a forced neighbour below or above us so we stop and put this node in both lists with one connection between the current node and this node.

We then execute this procedure to left. 

Afterwards we carry out a vertical scan which is similar to the horizontal scan but then up and down.

After that we go to the next step: a diagonal scan. 
This works as follows: for each of the top right, top left, bottom right, and bottom left corners, we shift one step at a cost of 1.5 until we find a forced neighbour around us or when we find ourselves at the end of the grid at each of these diagonal scans. 
We also do a horizontal and vertical scan for each node. And make jump points if we find anything during the scan. 
We make a connection between the node found during the scan and the node where we are currently standing and a connection with the node where we are currently and the current node.

https://github.com/Neleab/JumpPointSearchPlus-With-GoalBounding/blob/main/GIFJPS.gif?raw=true

3-Result

https://github.com/Neleab/JumpPointSearchPlus-With-GoalBounding/blob/main/Result.png?raw=true

4-Conclusion/Future work

Adding goal-bounding allows the jump point search to accelerate slightly, as it is able to ignore unnecessary nodes and operations. 
This can be an advantage, but also comes with the disadvantage that you cannot have a dynamic map, as you cannot add or remove nodes or edges.
What also can be added is to check whether there is another forced neighbour next to the founded jump point, for example if entering a long tunnel it would be better to create a jump point at the end of the tunnel than every tile in the tunnel.  

5-References

Witmer, N. (2013) ’A Visual Explanation of Jump Point Search’, zerowidth positive lookahead, May. Available at: https://zerowidth.com/2013/a-visual-explanation-of-jump-point-search.html (Accessed: 6 January 2023).

Hofkamp, A. (2015) ’Jump Point Search: Fast A* Pathfinding for Uniform Cost Grids’, gamedev.net, 31 October. Available at: https://www.gamedev.net/tutorials/programming/artificial-intelligence/jump-point-search-fast-a-pathfinding-for-uniform-cost-grids-r4220/ (Accessed: 6 January 2023).

Rabin, S. & Sturtevant, N. R.’Faster A* with Goal Bounding’, GameAIPro. Available at: http://www.gameaipro.com/GameAIPro3/GameAIPro3_Chapter22_Faster_A_Star_with_Goal_Bounding.pdf (Accessed: 6 January 2023).
