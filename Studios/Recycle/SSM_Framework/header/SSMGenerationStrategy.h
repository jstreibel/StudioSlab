/*
 *  SSMGenerationStrategy.h
 *  SSMFramework
 *
 *  Created by Joao Streibel on 07/12/2010.
 *  No rights reserved.
 *
 */

#ifndef SSM_STRATEGY
#define SSM_STRATEGY

class SSMGenerationStrategy {
public:
	typedef enum LOCATION {HOST, DEVICE} LOCATION;
protected:
	
	
	float *receiver;
	float *occluder;
	float *SSM;
	
	LOCATION location;
	bool powerOfTwoMaps;
	bool squareMaps;
	bool squareLight;
	bool generatesIntermediateData;
	int mapWidth, mapHeight;
	
public:
	void setReceiversPointer(float *receiver) {this->receiver = receiver;}
	void setOccludersPointer(float *occluder) {this->occluder = occluder;}
	void setSSMPointer(float *SSM) {this->SSM = SSM;}
	
	virtual void computeSSM() = 0;
	
	LOCATION getLocation() {return location;}
	bool usesPowerOfTwoMaps() {return powerOfTwoMaps;}
	bool usesSquareMaps() {return squareMaps;}
	bool usesSquareLight() {return squareMaps;}
	bool doesGenerateIntermediateData() {return generatesIntermediateData;}
	
};

#endif