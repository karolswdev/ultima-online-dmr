#!/bin/bash

echo $PWD
echo "Running docker run -v $PWD/src/pkg:/pkg -v $PWD/src/scripts:/scripts ultimadmr/dmr-ecompiler"
docker run -v $PWD/src/pkg:/pkg -v $PWD/src/scripts:/scripts ultimadmr/dmr-ecompiler

exit $?