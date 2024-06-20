# Overview

This assignment serves as an introduction to programming in C. As the first assignment, the programs that you will write are smaller than you may be used to and smaller than what is to come. Take the opportunity to emphasize a focused understanding, to explore the use of new tools to aid in your programming, and to reinforce concepts from prior coursework and projects. Challenge yourself to develop a quality solution that goes beyond a solution that just works.

# Course Learning Objectives

This assignment addresses the following course learning objectives.

 Direct
- Read and write complex programs in the C programming language.
- Transition to upper-division coursework where programming is a tool used in the exploration of course-specific content.
- Transition from following stepwise, prescribed decompositions to independent problem solving.

 Indirect
- Use standard UNIX user-level commands and the UNIX software development environment.

# The Tasks

There are two separate tasks listed below. Each task asks that you write a program to emulate a program commonly found in Unix installations.

### Task 1 - word count

The `wc` program is a utility that counts the number of lines, words, and bytes in its input.

__Definitions:__
- word: whitespace-delimited sequence of characters (use `isspace()` to check for whitespace characters)
- line: sequence of bytes terminated by a newline character (`\n`)

Write a program (with `main` function in a file named `word_count.c`) that takes a single (optional) command-line argument specifying a file name. The program must read the contents of the file (read from `stdin` if no file is specified) and print the number of lines, number of words, and number of bytes. Print these values as integers, with a space between each integer followed by a single newline character.

__Note__: Your program must not dynamically allocate memory (e.g., by using one of the `_alloc` functions). It should read a single character at a time. For full credit, a valgrind analysis of your program must not report any issues.

### Task 2 - unique lines

The `uniq` program is a common utility available in many Unix installations. This program echoes (to `stdout`) its input, but will print only a single instance of consecutive matching lines.

Sample input:
```
This is a line.
This is a line.
This is a line.
This is a different line.
This is a line.
```

Expected output from above input:
```
This is a line.
This is a different line.
This is a line.
```

Write a program (with `main` in a file named `uniq.c`) that takes a single (optional) command-line argument specifying a file name. The program must read the contents of the file (read from `stdin` if no file is specified) and print the contents of the input stream to `stdout` such that only a single instance of consecutive matching lines is printed. 

__Note__: your program must not explicitly dynamically allocate memory (e.g., by using one of the `_alloc` functions). It should use the `fgets()` function. You may assume a maximum line length of 512 bytes (make sure to account for the string termination character.) For full credit, a valgrind analysis of your program must not report any issues.

# Testing

On each git push to your repository, a sequence of preliminary tests will be automatically performed. These tests confirm that your output generally matches the output from the built-in `wc` and `uniq` commands. You may use the following steps to test interactively:

1. Create a text file containing test input

    `echo "test input text" > test_input`

3. Run your program, capture the output

    `./a.out test_input > test_output`

4. Run the built-in command using the same input file. Note that the built-in wc program includes the file name in its output. The awk filter here produces integer-only output.

    `wc test_input | awk '{print $1,$2,$3}' > expected_output`

5. Compare your output with the expected output. The diff command should return no results (indicating an exact match between the two output files)

    `diff test_output expected_output`

# Deliverables

- Source Code - Push all relevant source code to your repository.
- Include your full name and Cal Poly email address in a `README.md` file within your repository.
- For full credit, your git commit history must be sufficiently detailed to illustrate how you approached the assignment, addressed bugs, etc. Commit often, with descriptive commit labels. For example: "initial structures and function prototypes", "implemented feature xyz", "solved resource leak in function xyz()"  There is no fixed minimum number of commits. As one example guideline, you will not be eligible for full credit if your submitted git repository includes just 2-3 commits where large blocks of code are added in each commit.

# Rubric

### Criteria:

- word count: functionality
- word count: program style
The source code follows general best practices in terms of line length, function decomposition (non-duplication), naming principles, etc.
- unique: functionality
- unique: program style
The source code follows general best practices in terms of line length, function decomposition (non-duplication), naming principles, etc.
- Valgrind Check:
valgrind reports no issues.
