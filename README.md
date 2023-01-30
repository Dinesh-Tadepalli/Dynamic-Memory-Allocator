# Project 3: Memory Allocator

This project focuses on one of the major themes of Machine Organization and Programming classes taught for many years at universities all over the world and has gained a reputation for being a challenging project. 

To get started, first make sure you understand the basic operations of memory allocators. If necessary, rewatch the lecture videos and review section 9.9 of the CSAPP textbook. You will need to learn this material in preparation for the final. This project is designed to reinforce that learning.

## Learning Goals

The purpose of this project is to help you understand the nuances of building a memory allocator and further increase your C programming skills by working more with pointers.

## Specifications

For this assignment, we will provide some very basic starter code which you will complete to implement a memory allocator. You must implement three functions: `Mem_Init(...)`, `Mem_Alloc(...)`, and `Mem_Free(...)`. You may **not include any libraries beyond what is in the starter files (calling malloc() or free() will only cause you to fail the tests)**.

Starter code for these three functions can be found in the file [mem.c](src/mem.c). Please see this template file for the exact function descriptions and specifications.

Below is a list of the specifications your memory allocator must meet:

* You will be writing your memory allocator to run on **64-bit** machine. This means pointers returned by `Mem_Alloc(...)` must satisfy a 16-byte alignment (the value of the pointer returned to the user must be divisible by 16). This also means that the minimum block size is 16 bytes. 
* Headers can be a maximum of 8 bytes (no minimum).
* Even if you do not use the footers, you must account for them in an allocated block. If you choose your headers to be an `unsigned int` and maintain a 16 byte minimum block size, the maximum payload that can fit in a minimally sized block is 8 bytes (4 bytes for header, 8 byte payload, 0 bytes for padding, 4 bytes for footer even when not used). 
* Headers must be located immediately before the pointer returned to a user. For a the following examples, consider `malloc(8)` returning a pointer whose value is 0x40.
    * If you use an `unsigned int` to store headers, then the header for the current block must be located at 0x3c.
    * If you use a `unsigned long` to store headers, then the header for the current block must be located at 0x38.

* You must implement a first fit policy, and split the block where possible. What does "where possible" mean you might ask. If your first fit policy finds a block that is large enough to satisfy the request, it is possible to split that block if and only if after splitting, both blocks are **at least** the minimum block size.
* When allocating a block, you must a minimal amount of padding.
* You must immediately coalesce free blocks.

### Required Functions

* Mem\_Init(...) initializes the provided heap space so that future calls run without error.
* Mem\_Alloc(...) a function you implement that a user application can call to request space on the heap.
* Mem\_Free(...) a function you implement that a user application can call to signal it is done with a block of memory on the heap.

### Libraries

All libraries needed for the completion of this have been included for you already. It is not permitted to use any external C libraries beyond those present in the starter files.

### Git Commit History

Please develop and checkpoint your code incrementally. If you only wait till that last minute to commit and push all your changes, you are not taking advantage of the useful checkpointing features Git provides. When grading, we will be looking at your git log for meaningful commit messages. I recommended pushing a commit after you  make a major change to a function. 

## Files

[mem.h](inc/mem.h): Header file for the memory allocator functions. Your code should work with the original version of this file.

[header.h](inc/header.h): Header file for defining the size of your headers (and footers). **You may edit this file.**

[mem.c](src/mem.c): Main file where you will be implementing your functions. 

[Makefile](Makefile): Simple makefile to make compiling and testing easier.

It is strongly encouraged that you thoroughly test your memory allocator implementations. Feel free to share your test inputs on Piazza.

## Compiling and Running on Your Own

### Compiling

If you decide to test code on a CSL machine (either in-person in the labs or via ssh), you can use the following commands to test your code:

`make` 

This command will build object files for all the driver files in the `drivers/` directory. This means that if you want to write your own tests, you simply need to write a program and place it in the `drivers/` directory. Your code must compile without warnings or errors for the tests originally provided in this directory. 

### Running

To see the output of a specific test, simply invoke the `.out` file that was generated by the previous command. E.g. The first test you should try can be invoked by:

`./init.c.out`

## Strategy

You should start by reviewing the lectures on dynamic memory allocation and chapter 9.9 in the CSAPP textbook. Roughly, you may work through the following steps to help to finish the project. Note, that these steps are merely a suggestion.

### Step 1: Decide what your implementation type

After reviewing the lecture content, you should decide if you wish to implement an implicit or explicit list for this project. Both implementations can lead to getting all the points on this assignment, however, if you are interested in the attempting the extra credit, I would suggest trying to implement an explicit list. While the explicit list implementation is more difficult, it quite a bit of effort to switch from an implicit list to an explicit list. So your first decision should be based on whether you think you'll have enough time to implement an explicit list in it's entirety.

### Step 2: Decide what your blocks will look like

After you have decided on your implementation, now you must decide on the structure of your blocks. You have the following questions to answer:

* How large should my headers be?
    * In class we presented headers as 4 byte unsigned integers. This is what the `Header` type has been initialized to in [header.h](inc/header.h), so if you wish to stick with this implementation, you need not change this file. If you do wish to change the number of bytes used for the header, you may do so in this file.

* What do my allocated blocks look like?
    * I recommend drawing diagrams of what blocks returned by `malloc(1)`, `malloc(8)`, `malloc(9)`, and `malloc(16)` look like so you can understand what is required of you. You should also think about what the maximum payload size for a minimally sized block. If you come to use in OH, we will ask to see these diagram **before** we provide any useful feedback.

* What do my free blocks look like?
    * I recommend drawing diagram of what a free block looks like in your implementation. If you come to use in OH, we will ask to see these diagram **before** we provide any useful feedback. The primary point of confusion here will stem from whether you chose to implement an explicit list or not. For example, if you choose to implement a doubly-linked, explicit list, the minimum block size for your free blocks will be **different** than the minimum block size for your allocated blocks. 

### Step 3: Initializing the heap

* Your first coding task is to implement the `Mem_Init(...)` function. Please see the [mem.c](src/mem.c) for a more detailed description of this function. The key idea here is that this function marks the heap as a single free block before returning. After implementing this function, you should pass the `./init.c.out` test case. After completing this step, I **highly recommend** you commit and push your work so far.

### Step 4: Allocating a block

* When a request comes in, you must first check that the requested size is valid (non-zero). After validating this, you must determine the size of a block that is appropriate to satisfy this request. When determining this size, you must account for the size of the header, space for the footer (**even if you aren't using footers**), and any padding you might need for the alignment requirement. After determining this value, you must search the heap space **in address order** (first fit) to find a free block that can satisfy this request. Once found, you must determine if the block can be split, and split if you can. After implementing this function, you should pass the `./alloc_15.c.out` test case. After completing this step, you commit and push your work.

### Step 5: Freeing a block

* Before anything, you must validated that the user has given you a pointer that can be freed. See the comments in the `Mem_Free(...)` function for what these checks are. After validating the pointer to be freed, you'll want to mark the header of this block to be free. After this, you must determine if this free block can be coalesced. The block can be coalesced if there is a free block before it, a free block after it, or both. If there is a neighboring free block, perform the coalesce. After implementing this function, you should pass the `./free_15.c.out` test case. After completing this step, you commit and push your work.
 
### Step 6: Writing more tests

* These are the basic tests we provide you intitially. The teaching staff may decide to release more tests later on, but it is strongly encouraged that you write more tests to expose corner cases. 

## Turn in

As mentioned in class, you **must** tag your submission to ensure that we grade it. Once you are satisfied with your progress, you can submit using the commands shown below. Since many people submitted early, they wanted to go back and modify their solution after tagging it. To ensure that the teaching staff doesn't have to manually go through and delete tags, we will allow multiple submissions. I.e., you commit histroy can contain multiple tags. However, the tag must start with the prefix `FINAL` for us to count it as a submission. As an example, let's say you submitted your assignment and pushed the tag `FINAL000`. However, after reading some piazza posts, yourealize that you need to go an change your solution. Once you have made the chages, added, committed, and pushed them, please push **another** tag, `FINAL001`. When we grade, we will checkout to the tag with the highest number in the suffix. So for your first submission, you would use that following commands to push the changes to the source files:

`git add <SOURCE_FILES>`

`git commit -m <SOME_MESSAGE>`

`git push origin main`

And then you would use the following commands to push the tag to mark your submission as ready to be graded:

`git tag -a "FINAL000" -m "Submitting my work"`

`git push origin FINAL000`

And if you needed to make changes after the deadline, you could submit your next attempt with:

`git tag -a "FINAL001" -m "Ope, made a mistake in my initial submission"`

`git push origin FINAL001`

## Style

Please follow the [Style Guide](docs/Style_Guide.pdf) (same as the one linked on the Canvas Homepage).

## Extra Credit (up to 20%)

### Prologue and Epilogue Blocks (3%)

You'll notice that all the tests in the `drivers/` directory `malloc()` a request larger than the heap space provided to `Mem_Init(...)`. This is to allow you room to place in prologue and epilogue blocks. These blocks are often used to improve the performance of memory allocators, as they make coalescing easier.

#### Prologue Block (2%)

Immediately before the header for the first block, to get credit, you must write in a 0-byte payload block. For a prologue block to be effective, you must write in values to both the header and the footer. 
* Writing the header requires you to write the minimum block size (16) to the address 16 bytes before the first header, and mark it as allocated.
* Writing the footer requires you to copy the header at the end of the block (i.e., `sizeof(Header)` bytes before the first header).

#### Epilogue Block (1%)

At the end of the heap space you are given an extra `sizeof(Header)` bytes to write the epilogue block header. This should be set to `0x1` to denote the end of the heap.

### Footers (4%)

A footer is an exact copy of a header, just located at the end of an allocated block. To get credit for this section, every allocated block that every exists must have a copy of the header at the end of the block. 

### Memory Utiization (6%)

The extra credit for this section is based on how well you utilize the heap space. The amount of extra credit here will be determined by what thresholds you cross from `./utilization.c.out`. If you satisfy at least X requests, you will recieve 3% out the 6% extra credit in this section. If you satisfy at least Y  requests, you will recieve all 6% extra credit in this section.

These numbers are: X=100 and Y=200. You will only receive the extra credit in this section if your solution passes all of the correctness tests first.

### Performance (7%)

You'll note that running stress.out yields an estimate of the runtime of your program. Performance is important as a memory allocator is used by a large number of programs. Your goal is to do better than our implementation by optimizing the allocation and coalescing time.

To achieve this, you will need to implement an explicit list to manage the blocks in the heap. If you beat our solutions time on `./stress.c.out`, you will recieve 5% of the 7% in this section. The remaining 2% is gained on a competitive scale. So if your solution is the fastest in class, you get the final 2% extra credit. See the following for an example of how the extra credit for this section might be calculated: 

```C
extra_credit = 0.0;
if (your_runtime < ta_runtime){
    bonus_points += 5 + [2.5 * (2 - (your_solution - fastest_student_solution)/(slowest_student_solution - fastest_student_solution))];
}
```

The performance of the TA solution will be released at a later time. You will only receive the extra credit in this section if your solution passes all of the correctness tests first.

## References

* CSAPP Section 9.9

## Authors and acknowledgment
This project was originally developed by Bujji Setty in the Summer of 2022. 

