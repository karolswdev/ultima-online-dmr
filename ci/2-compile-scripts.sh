#!/bin/bash

echo $pwd
echo "Running docker run -v $pwd/src/pkg:/pkg -v $pwd/src/scripts:/scripts ultimadmr/dmr-ecompiler"
docker run -v $pwd/src/pkg:/pkg -v $pwd/src/scripts:/scripts ultimadmr/dmr-ecompiler