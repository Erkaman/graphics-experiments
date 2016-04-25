#pragma once

class WhirlPatternCreator {

private:

    int radius;

     const int NORTH;
     const int EAST;
     const int SOUTH;
     const int WEST;

    /**
     * The current direction of the pattern.
     */
    int direction;

    /*
     * Move D square in direction.
     */
    int D;

    /*
     * Current path length.
     */
    int i;

    /*
     * The number of times the direction has changed.
     */
    int directionChanges = 0;


    /**
     * Current pos of the pattern.
     */
    int x;
    int z;

    int outputX;
    int outputZ;

    int abs(int i) {
	return i < 0 ? -i : +i;
    }

    int my_max(int a, int b) {
	return a > b ? a : b;
    }

    bool reachedEndOfPattern() {
	int d = my_max(abs(x), abs(z));
	return d > radius;
    }


public:
    WhirlPatternCreator():
	NORTH(0),
	EAST(1),
	SOUTH(2),
	WEST(3) {

    }

    void Start(int radius) {
	assert(radius > 0);


	this->x = 0;
	this->z = -1;


	this->radius = radius;
	this->direction = NORTH;
	this->D = 2;
	this->i = 0;
	this->directionChanges = 0;
    }

    bool Next() {

	/*
	 * Move to next pos.
	 */
	if(this->direction == NORTH) {
	    ++z;
	} else if(this->direction == SOUTH) {
	    --z;
	}  else if(this->direction == WEST) {
	    ++x;
	}  else if(this->direction == EAST) {
	    --x;
	}

	if(reachedEndOfPattern()) {
	    /**
	     * If end reached, we always return zero.
	     */
	    outputX = 0;
	    outputZ = 0;
	    return false;
	}

	/**
	 * Output pos.
	 */
	outputX = x;
	outputZ = z;

	// increase path length.
	++i;

	// change direction.
	if(i == D) {

	    if(this->direction == NORTH) {
		this->direction = EAST;
	    } else if(this->direction == SOUTH) {
		this->direction = WEST;
	    }  else if(this->direction == WEST) {
		this->direction = NORTH;
	    }  else if(this->direction == EAST) {

		this->direction = SOUTH;
	    }
	    ++directionChanges;
	    i = 1;

	    // increase path length if necessary.
	    if((directionChanges % 2) == 0) {
		++D;
	    }
	}

	return true;
    }

    int GetX() {
	return this->outputX;
    }

    int GetZ() {
	return this->outputZ;
    }


};
