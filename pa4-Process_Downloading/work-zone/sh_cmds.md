rm file1 file2 file3 file4 file5 file6 file7 file8
valgrind --leak-check=yes ./a4download test_urls.txt 10
time valgrind --leak-check=yes ./a4download test_urls.txt 10
time ./a4download test_urls.txt 10
