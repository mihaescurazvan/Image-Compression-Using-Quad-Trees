# Image-Compression-Using-Quad-Trees

The purpose of this assignment was to implement and use quad trees for the compression of PPM images. Three tasks were required to be solved:
    * Task 1: Building the quad tree and determining statistics based on it.
    * Task 2: Compressing an image in PPM format using the previously created quad tree.
    * Task 3: Reconstructing the original image from a file resulting from compression.

In the following sections, I will present my solution for solving this assignment. I have created four types of structures:
    * **rgb**: Structure for a pixel.
    * **quadtree**:  Structure for the quad tree containing an **rgb** field, links to its four children, and a **size** field, which stores the image's size for leaf nodes.
    * **node** : Structure for a cell in the queue.
    * **queue** : Structure for the queue.

If the first command-line argument is '-c1' or '-c2', I call the **read_ppm**, function, which reads a PPM file, and then I call the **build_quadtree** function to construct the quad tree. This function works as follows:
  * I divide the image into 4 quarters.
  * For the large image, I calculate the color averages using the **mean_red**, **mean_green**, **mean_blue**, and **mean_color** functions.
  * If the color averages are less than the compression factor, I make the current node a leaf and set its fields to the calculated average colors.
  * Otherwise, I recursively call the function with the new images until I reach images with color averages smaller than the factor.

If the first argument is "-c1":
    * I display the number of levels in the quad tree using the **count_levels** function.
    * I display the blocks of the image for which the similarity score of the pixels is less than or equal to the provided factor using the **count_blocks** function.
    * I find the size of the square side for the largest area in the image that remains undivided using the **max_size** function.

If the first argument is "-c2", I write the image's dimensions to the output file, and then I call the **compress_image** function, which writes the tree to the file. The **compress_image** function traverses the tree in breadth-first order using a queue and writes the type of each node (1 if it's a leaf, 0 otherwise) to the file. If the node is a leaf, it writes its colors to the file.

If the first argument is "-d", I read a quad tree from the input file and call the **build_quadtree_from_file** function, which creates the tree from the compressed file. The **build_quadtree_from_file** function adds nodes to the tree level by level using a queue. After this, the **decompress_image** function is called, which performs a breadth-first traversal of the tree. For leaf nodes, it colors the image pixels with the colors of the current node. For internal nodes, it recursively calls the function for each child.

Finally, I write the resulting pixel matrix to the output file after calling the **decompress_image** function.
