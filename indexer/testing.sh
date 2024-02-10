
echo "Integration testing for indexer module."
echo 
echo "=========================================================="
echo
echo "Testing indexer (error-handling)"

echo "Error-handling: Insufficient arguments"
# no arguments
echo -e "\ntesting with no arguments"
./indexer
echo

# one argument
echo -e "\ntesting with one argument"
./indexer ../tse-output/letters-depth-1
echo

echo "Error-handling: Too many arguments"
# three or more arguments
echo -e "\ntesting with three or more arguments"
./indexer ../tse-output/letters-depth-1 arg2 arg3
./indexer ../tse-output/letters-depth-2 arg2 arg3 arg4 arg5 arg6
echo

echo "Error-handling: Testing indexer on invalid pagedirectory"
# non-existent path
echo -e "\ntesting with invalid pageDirectory (non-existent path)"
./indexer ../tse-output/letters-depth-9 ../tse-output/letters-depth-9/index.ndx
echo

# not a crawler directory
echo -e "\ntesting with invalid pageDirectory (not a crawler directory)"
mkdir ../tse-output/not_crawler
touch ../tse-output/not_crawler/not_crawled.txt
./indexer ../tse-output/not_crawler ../tse-output/not_crawler/index.ndx
# check if extra file created
FILE=../tse-output/not_crawler/index.ndx
if [[ -f "$FILE" ]]; then
    echo -e "\nERROR: $FILE created"
fi
# cleanup directory
# rm -r ../tse-output/not_crawler
echo

# invalid indexFile (non-existent path)
echo -e "\ntesting with invalid indexFile (non-existent path)"
./indexer ../tse-output/letters-depth-1 ../tse-output/letters-depth-100/index.ndx
echo

# invalid indexFile (read-only directory)
echo -e "\ntesting with invalid indexFile (read-only directory)"
chmod -w ../tse-output/letters-depth-1
./indexer ../tse-output/letters-depth-1 ../tse-output/letters-depth-1/index.ndx
# reset permissions
chmod +w ../tse-output/letters-depth-1
echo

# invalid indexFile (existing, read-only file)
echo -e "\ntesting with invalid indexFile (existing, read-only file)"
touch ../tse-output/letters-depth-1/index.ndx
chmod -w ../tse-output/letters-depth-1/index.ndx
./indexer ../tse-output/letters-depth-1 ../tse-output/letters-depth-1/index.ndx
# reset permissions
chmod +w ../tse-output/letters-depth-1/index.ndx
# rm ../tse-output/letters-depth-1/index.ndx
echo

### Test indexer on several valid pageDirectories, validated by indextest ###
echo "Testing indexer on letters at depth 0, 1, 2, 3, 4"
echo "Testing indexer on letters at depth 0 file"
echo -e "\ntesting on pageDirectory ../tse-output/letters-depth-0 ..."
./indexer ../tse-output/letters-depth-0 ../tse-output/letters-depth-0/index.ndx
./indextest ../tse-output/letters-depth-0/index.ndx ../tse-output/letters-depth-0/index_new.ndx
var="$(diff <(sort ../tse-output/letters-depth-0/index.ndx) <(sort ../tse-output/letters-depth-0/index_new.ndx))"
if [ -z "$var" ]
then
      echo -e "\noutput matches!"
else
      echo -e "\nError: OUTPUT DOES NOT MATCH"
fi
# cleanup
# rm ../tse-output/letters-depth-0/index.ndx ../tse-output/letters-depth-0/index_new.ndx
echo

echo "Testing indexer on letters at depth 1 file"
echo -e "\ntesting on pageDirectory ../tse-output/letters-depth-1 ..."
./indexer ../tse-output/letters-depth-1 ../tse-output/letters-depth-1/index.ndx
./indextest ../tse-output/letters-depth-1/index.ndx ../tse-output/letters-depth-1/index_new.ndx
var="$(diff <(sort ../tse-output/letters-depth-1/index.ndx) <(sort ../tse-output/letters-depth-1/index_new.ndx))"
if [ -z "$var" ]
then
      echo -e "\noutput matches!"
else
      echo -e "\nError: OUTPUT DOES NOT MATCH"
fi
# cleanup
# rm ../tse-output/letters-depth-1/index.ndx ../tse-output/letters-depth-1/index_new.ndx
echo

echo "Testing indexer on letters at depth 2 file"
echo -e "\ntesting on pageDirectory ../tse-output/letters-depth-2 ..."
./indexer ../tse-output/letters-depth-2 ../tse-output/letters-depth-2/index.ndx
./indextest ../tse-output/letters-depth-2/index.ndx ../tse-output/letters-depth-2/index_new.ndx
var="$(diff <(sort ../tse-output/letters-depth-2/index.ndx) <(sort ../tse-output/letters-depth-2/index_new.ndx))"
if [ -z "$var" ]
then
      echo -e "\noutput matches!"
else
      echo -e "\nOUTPUT DOES NOT MATCH"
fi
# cleanup
# rm ../tse-output/letters-depth-2/index.ndx ../tse-output/letters-depth-2/index_new.ndx
echo

echo "Testing indexer on letters at depth 3 file"
echo -e "\ntesting on pageDirectory ../tse-output/letters-depth-3 ..."
./indexer ../tse-output/letters-depth-3 ../tse-output/letters-depth-3/index.ndx
./indextest ../tse-output/letters-depth-3/index.ndx ../tse-output/letters-depth-3/index_new.ndx
var="$(diff <(sort ../tse-output/letters-depth-3/index.ndx) <(sort ../tse-output/letters-depth-3/index_new.ndx))"
if [ -z "$var" ]
then
      echo -e "\noutput matches!"
else
      echo -e "\nOUTPUT DOES NOT MATCH"
fi
# cleanup
# rm ../tse-output/letters-depth-3/index.ndx ../tse-output/letters-depth-3/index_new.ndx
echo


echo "Testing indexer on letters at depth 4 file"
echo -e "\ntesting on pageDirectory ../tse-output/letters-depth-4 ..."
./indexer ../tse-output/letters-depth-4 ../tse-output/letters-depth-4/index.ndx
./indextest ../tse-output/letters-depth-4/index.ndx ../tse-output/letters-depth-4/index_new.ndx
var="$(diff <(sort ../tse-output/letters-depth-4/index.ndx) <(sort ../tse-output/letters-depth-4/index_new.ndx))"
if [ -z "$var" ]
then
      echo -e "\noutput matches!"
else
      echo -e "\nOUTPUT DOES NOT MATCH"
fi
# cleanup
# rm ../tse-output/letters-depth-4/index.ndx ../tse-output/letters-depth-4/index_new.ndx
echo

# Run valgrind on both indexer and indextest to ensure no memory leaks or errors 
echo -e "\nTest for memory leaks on letters at depth 0 file"
echo -e "\nrunning valgrind in indexer to check for memory leaks"
valgrind --leak-check=full --show-leak-kinds=all -s ./indexer ../tse-output/letters-depth-0 ../tse-output/letters-depth-0/index.ndx
echo -e "\nrunning valgrind in indextest to check for memory leaks"
valgrind --leak-check=full --show-leak-kinds=all -s ./indextest ../tse-output/letters-depth-0/index.ndx ../tse-output/letters-depth-0/index_new.ndx
# cleanup
# rm ../tse-output/letters-depth-0/index.ndx ../tse-output/letters-depth-0/index_new.ndx
echo

echo -e "\nTest for memory leaks on letters at depth 1 file"
echo -e "\nrunning valgrind in indexer to check for memory leaks"
valgrind --leak-check=full --show-leak-kinds=all -s ./indexer ../tse-output/letters-depth-1 ../tse-output/letters-depth-1/index.ndx
echo -e "\nrunning valgrind in indextest to check for memory leaks"
valgrind --leak-check=full --show-leak-kinds=all -s ./indextest ../tse-output/letters-depth-1/index.ndx ../tse-output/letters-depth-1/index_new.ndx
# cleanup
# rm ../tse-output/letters-depth-1/index.ndx ../tse-output/letters-depth-1/index_new.ndx
echo

echo -e "\nTest for memory leaks on letters at depth 2 file"
echo -e "\nrunning valgrind in indexer to check for memory leaks"
valgrind --leak-check=full --show-leak-kinds=all -s ./indexer ../tse-output/letters-depth-2 ../tse-output/letters-depth-2/index.ndx
echo -e "\nrunning valgrind in indextest to check for memory leaks"
valgrind --leak-check=full --show-leak-kinds=all -s ./indextest ../tse-output/letters-depth-2/index.ndx ../tse-output/letters-depth-2/index_new.ndx
# cleanup
# rm ../tse-output/letters-depth-2/index.ndx ../tse-output/letters-depth-2/index_new.ndx
echo

echo -e "\nTest for memory leaks on letters at depth 3 file"
echo -e "\nrunning valgrind in indexer to check for memory leaks"
valgrind --leak-check=full --show-leak-kinds=all -s ./indexer ../tse-output/letters-depth-3 ../tse-output/letters-depth-3/index.ndx
echo -e "\nrunning valgrind in indextest to check for memory leaks"
valgrind --leak-check=full --show-leak-kinds=all -s ./indextest ../tse-output/letters-depth-3/index.ndx ../tse-output/letters-depth-3/index_new.ndx
# cleanup
# rm ../tse-output/letters-depth-3/index.ndx ../tse-output/letters-depth-3/index_new.ndx
echo

echo -e "\nTest for memory leaks on letters at depth 4 file"
echo -e "\nrunning valgrind in indexer to check for memory leaks"
valgrind --leak-check=full --show-leak-kinds=all -s ./indexer ../tse-output/letters-depth-4 ../tse-output/letters-depth-4/index.ndx
echo -e "\nrunning valgrind in indextest to check for memory leaks"
valgrind --leak-check=full --show-leak-kinds=all -s ./indextest ../tse-output/letters-depth-4/index.ndx ../tse-output/letters-depth-4/index_new.ndx
# cleanup
# rm ../tse-output/letters-depth-4/index.ndx ../tse-output/letters-depth-4/index_new.ndx
echo



