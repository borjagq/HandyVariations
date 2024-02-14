#! /bin/zsh

#==============================================================================
# title         rename_rdm_bg.sh
# description   Rename all files in $DIRECTORY to the expected HandyVariations
#               name format for random backgrounds.
# author		Borja García Quiroga <garcaqub@tcd.ie>
# date          2023-09-28
# version       1.0
# usage		    zsh mkscript.sh $DIRECTORY
# zsh_version   5.9 (x86_64-apple-darwin22.0)
#
# Copyright (c) Borja García Quiroga, All Rights Reserved.
#
# The information and material provided below was developed as partial
# requirements for the MSc in Computer Science at Trinity College Dublin,
# Ireland.
#==============================================================================

# Get the parameters.
DIRECTORY=$1

# Declare the function to print progress bars.
my_function () {
  func_result="some result"
}

# Check that something was passed as directory.
if [ -z "$DIRECTORY" ]; then
    echo "Error: You must pass a directory.";
    echo "Usage: zsh mkscript.sh DIRECTORY"
    return;
fi;

# Check that the path is a real directory.
if [ ! -d "$DIRECTORY" ]; then
    echo "Error: You must pass an existing directory.";
    return;
fi;

# Get the number of files in the directory.
NUM_OF_FILES=$(($(find "$DIRECTORY" -type f -maxdepth 1 -mindepth 1 -not -path '*/.*' | wc -l)))

# Ask for confirmation.
echo "Attention: $DIRECTORY has $NUM_OF_FILES files."
echo "Continuing forward will rename all of them."
echo "Are you sure? [y/n]"
read CONT
while [[ "$CONT" != "y" && "$CONT" != "n" ]];
do
    echo "Are you sure? [y/n]"
    read CONT
done;

# If the user said no, quit.
if [ "$CONT" = "n" ]; then
    return;
fi;

# Set the counter to zero.
COUNT=0

# Store the current print to know when to update.
LAST_PROGRESS=-1

# Iterate through the files in the directory.
for FILE in "$DIRECTORY"/*; do

    # Make sure that if it is a dir we skip it.
    if [ ! -f "$FILE" ]; then
        continue;
    fi

    # Get the new name of the file.
    ID=$(printf "%012d" $COUNT)
    EXT="${FILE##*.}"
    NAME="rdm_bg_$ID"
    DIR=$(dirname "${FILE}")

    # Rename the file.
    mv "$FILE" "$DIR/$NAME.$EXT"

    # Increment the counter.
    COUNT=$(($COUNT + 1))

    # Get the progress bar that we will print.
    PROGRESS="$COUNT.0"
    PROGRESS=$(($PROGRESS / $NUM_OF_FILES * 50))
    PROGRESS=${PROGRESS%.*}

    # Check if we need to update the bar.
    if (( $PROGRESS != $LAST_PROGRESS )); then
        BAR="Renaming: ["
        for I in {1..50}
        do
            if (($PROGRESS > $I)); then
                BAR="$BAR#"
            else
                BAR="$BAR "
            fi;
        done
        BAR="$BAR]"
        echo -ne "$BAR"\\r
    fi;

done

echo "Done!                                                             "
