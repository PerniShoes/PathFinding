# Pathfinding algorithm using A*
Due do struggles with git I decided to upload only my own files in this repo for now.
Other files used are the same as in the previous projects, specifically: 
- Engine project

Original video showcase:  
https://www.youtube.com/watch?v=ChtlBH_61W8  
Improved algorithm and a small bug fix:  
https://www.youtube.com/watch?v=1MXiVquZZWY  

Comparision of the first version of the algorithm:  
<img width="280" height="250" alt="Screenshot_2" src="https://github.com/user-attachments/assets/792b54d6-b3d3-4609-bebb-1bac1de45431" />  
And the improved one:    
<img width="284" height="253" alt="Screenshot_1" src="https://github.com/user-attachments/assets/93c71e7e-1762-40cb-b4c5-5ed689f9ea02" />

The way it works now is that instead of going through all the optimals paths  
"one step at a time" it goes through the first one found until it's not optimal  
then it switches to the second one and so on. So if the last path found is the   
shortest one, then both version are similar or the same. But if any of the previous  
ones is the shortest, it checks the paths only up to that one
