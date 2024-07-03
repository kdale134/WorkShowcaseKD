# CS-300-DSA-Analysis-and-Design

<b>What was the problem you were solving in the projects for this course?</b>
- Throughout this course we were analyzing the comparative efficiency and function between three data structures (BSTs, Vectors, and Hashtables) when reading a .csv file and formatting the output correctly. The program was intended to read various courses and their information from a file, and organize those courses into one of the data structures based on client needs. 

<b>How did you approach the problem? Consider why data structures are important to understand.</b>
- I approached the problem by considering which data structure had the most efficient worst case runtime. This was important for this instance as the file being read in contained course data and their respective ID's. This is important to know as course ID's often end in 0, which would nullify the best case result of a Hashtable, as each course would be hashed to the same, or a few buckets. When considering a Vector vs a BST, the client demanded the courses be displayed and their prerequisite shown, through the nature of the structure of a BST, it inherently organized the courses with their pre-reqs to the left of the desired course. This made it exceedingly easy to control and display course pre-reqs. A Vector would suffice, but I believe, if configured properly, a BST would produce the most efficient structure in this instance. 

<b>How did you overcome any roadblocks you encountered while going through the activities or project?</b>
- While going through the course, I utilized the provided codebases from the prior assignments dealt with in the construction of my project. They provided a solid foundation for how to establish each of the data structures, as well as read in a file. I also utilized StackExchange for the purposes of this project. 

<b>How has your work on this project expanded your approach to designing software and developing programs?</b>
- This course forced me to become much more intimate with various data structures, as well as methods to analyze their respective strengths, weaknesses, and efficiencies. It was important for my continued journey as a developer to not only learn the implementation of these various data structures, but also the 'under-the-hood' function various algorithms take to operate. This will allow me to produce much more efficient code, as well as discern the proper data structure to utilize on a case-by-case basis. 

<b>How has your work on this project evolved the way you write programs that are maintainable, readable, and adaptable?</b>
- As the implementation of data structures and their functionality can be and look complicated, it becomes evermore important to write readable, properly configured code. In addition, provide commentation on that code so any developer coming after you, may easily understand and interpret the code before them. This is pivotal for the continued efficient production of code on development teams, and the timely delivery of functional secure products. 
