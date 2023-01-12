# JumpPointSearchPlus-With-GoalBounding 

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
