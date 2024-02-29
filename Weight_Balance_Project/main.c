/*
 *
 * WEIGHT AND BALANCE TOOL FOR CESSNA 172R.
 *
 * WEIGHT AND BALANCE DATA IS BASED ON N29AF PILOT OPERATING HANDBOOK.
 *
 * THE VALUES PROVIDED MAY VARY BASED ON AIRCRAFT MODEL NUMBERS. THIS IS FOR EDUCATIONAL
 * PURPOSES ONLY. 
 * 
 * YOU, AS PILOT IN COMMAND, ARE SOLELY RESPONSIBLE FOR ASSURING CORRECT DATA AND PROPER LOADING OF YOUR
 * AIRCRAFT PRIOR TO FLIGHT.
 *
 * PLEASE PERFORM YOUR OWN WEIGHT AND BALANCE CALCULATIONS USING THE EXACT
 * WEIGHT AND BALANCE DATA FOR YOUR AIRPLANE.
 *
 * CONSULT YOUR AIRCRAFT'S POH. ALL UNITS ARE IN
 * LBS, INCHES, AND GALLONS.
 *
 *  				CREATED ON: Feb 10, 2024
 *      				AUTHOR: Jesus Martinez
 */

#include  <stdio.h>
#include  <string.h>
#include  <stdlib.h>
#include  <stdint.h>
 /*****Define constant macros for all known constants and aircraft data************/
#define 	usefulLoad  806.7       //Useful load in lbs.
#define 	FUELWEIGHT 		6 				    //100LL weighs 6 lb/gallon                           
#define 	FUELBURN 		  8  			   //C172R burns approx. 8 gallons/hour                 
#define 	MAXWEIGHT 		2450     //Maximum Weight of a C172R                         
#define 	MINWEIGHT 		1656    //Minimum Weight of a C172R                        
#define 	CGUPPER 		   46    //Upper CG Limit based on CG Envelope Chart in the POH
#define 	CGLOWER 		   35   //Lower CG Limit based on CG Envelope Chart in the POH*/

/***********Create structures for all calculated weights, arms, and moments***********/
struct Weights
{
    int pilot;
    int copilot;
    int passengers;
    int baggageArea1;
    int baggageArea2;
    int empty;
    int fuel;
};

struct Arms
{
    int pilotArm;
    int copilotArm;
    int passengersArm;
    int fuelArm;
    int baggageArm1;
    int baggageArm2;
    int emptyArm;
};

struct Moments
{
    int pilotMoment;
    int copilotMoment;
    int passMoment;
    int fuelMoment;
    int bagMoment1;
    int bagMoment2;
    int emptyMoment;
};
/**************Create an enum for the Pilot and Copilot's Height***********************/
enum PilotHeight
{
    NOPILOT = 0,
    SHORT   = 1,
    AVERAGE = 2,
    TALL    = 3
};

/*Declare the functions to calculate weight and balance and to check for conditions*/
int 	 sumMoments(struct Moments moment);
int 	 calculateWeight(struct Weights weight);
int 	 determinePilotArm();
int 	 determinecoPilotArm();
void 	checkBalance(int CG);
void 	checkWeight(int totalWeight, int rampWeight, int fuel);
void 	generateTable(struct Weights weight, struct Arms arm, struct Moments moment, int rampWeight, int takeoffWeight, int CG);
void  clearScreen();
void  WeightBalance();

/*Main function, where the program begins.*/
int main(void)
{
    /*Clear the screen at the start if the program.*/
    clearScreen();
    /*Call the weight and balance function to begin calculating.*/
    WeightBalance();
    return 0;
}


void WeightBalance()
{
    /*Declare all variables to be used and structure variables*/
    int gallonsFuel, rampWeight, usedWeight, totalMoments, CG;

    struct Weights weight;  struct Arms arm;   struct Moments moment;

    clearScreen();

    /********Get all of the known weights, and get the total ramp weight**********************/
    weight.empty = 1650; 
    printf("Enter the pilot's weight: ");
    scanf_s("%d", &weight.pilot);
    printf("Enter the copilot's weight: ");
    scanf_s("%d", &weight.copilot);
    printf("Enter the passengers' weight: ");
    scanf_s("%d", &weight.passengers);
    printf("Enter baggage compartment 1 weight: ");
    scanf_s("%d", &weight.baggageArea1);
    printf("Enter baggage compartment 2 weight: ");
    scanf_s("%d", &weight.baggageArea2);
    printf("Enter the amount of fuel onboard in gallons: ");
    scanf_s("%d", &gallonsFuel);
    weight.fuel = FUELWEIGHT * gallonsFuel;
    rampWeight  = calculateWeight(weight);
    printf("Now enter the arms for each item as they appear on the screen.\n");

    /******Calculate all of the Arms************/

    arm.pilotArm        = determinePilotArm();
    arm.copilotArm      = determinecoPilotArm();
    arm.passengersArm   = 73;
    arm.baggageArm1     = 95;
    arm.baggageArm2     = 123;
    arm.fuelArm         = 48;
    arm.emptyArm        = 39;

    /**********Calculate all of the Moments using the weights and arms of each item***********/
    moment.pilotMoment   = weight.pilot * arm.pilotArm;
    moment.copilotMoment = weight.copilot * arm.copilotArm;
    moment.passMoment    = weight.passengers * arm.passengersArm;
    moment.bagMoment1    = weight.baggageArea1 * arm.baggageArm1;
    moment.bagMoment2    = weight.baggageArea2 * arm.baggageArm2;
    moment.fuelMoment    = weight.fuel * arm.fuelArm;
    moment.emptyMoment   = weight.empty * arm.emptyArm;

    /*Calculate the sum of the moments.*/
    totalMoments = sumMoments(moment);

    /*********Calculate the Center of Gravity**********************/
    CG = totalMoments / rampWeight;

    /**********Calculate the useful load remaining****************/
    usedWeight = rampWeight - weight.empty;

    int fuelTO = 0.25 * FUELBURN; 			/*Taxi and runup is approx. 15 minutes.*/
    int weightTO = rampWeight - (FUELWEIGHT * fuelTO);

    /*Generate a Weight and Balnce Table and check for conditions.*/
    generateTable(weight, arm, moment, rampWeight, weightTO, CG);

    checkBalance(CG);

    checkWeight(usedWeight, rampWeight, gallonsFuel);

}

int calculateWeight(struct Weights weight)
{
    int grossWeight = 0;

    //loop through each member and its value to get the gross weight
    for (int i = 0; i < sizeof(struct Weights) / sizeof(int); ++i)
    {
        grossWeight += *((int*)&weight + i);
    }
    return grossWeight;
}

int sumMoments(struct Moments moment)
{
    int grossMoments = 0;
    //loop through each member and its value to get the sum of the moments.
    for (int i = 0; i < sizeof(struct Moments) / sizeof(int); ++i)
    {
        grossMoments += *((int*)&moment + i);
    }

    return abs(grossMoments);
}

/*Function that will determine the arm distance based on the Pilot's height range.*/
int determinePilotArm()
{
    int arm;
    enum PilotHeight choice;
    printf("Enter the corresponding number from the following options.\n");
    printf("Is the pilot short, average, or tall?\n");
    printf("1. short\n2. average\n3. tall\n");
    scanf_s("%d", (int*)&choice);
    switch (choice)
    {
    case SHORT: 	  arm = 34; break;
    case AVERAGE: 	arm = 37; break;
    case TALL: 		  arm = 46; break;
    case NOPILOT: 	arm = 0; printf("There must be a pilot. Please try again.\n"); exit(0); break;
    default: printf("Invalid Entry."); exit(0);	break;
    }
    return arm;
}

/*Function that will determine the arm distance based on the  Co-pilot's height range.*/
int determinecoPilotArm()
{
    int arm;
    enum PilotHeight choice;
    printf("Enter the corresponding number from the following options.\n");
    printf("Is the co-pilot short, average, or tall?\n");
    printf("0. No copilot\n1. short\n2. average\n3. tall\n");
    scanf_s("%d", (int*)&choice);
    switch (choice)
    {
    case SHORT: 	  arm = 34;	break;
    case AVERAGE: 	arm = 37;	break;
    case TALL: 		  arm = 46;	break;
    case NOPILOT: 	arm = 0;  break;
    default: printf("Invalid Entry."); exit(0);	break;
    }
    return arm;
}

/*Function will check if the CG is within the loading envelope.*/
void checkBalance(int CG)
{
    if (CG >= CGUPPER)
    {
        printf("CG out of range.\n");
        printf("CG is too high.\n");
    }
    else if (CG <= CGLOWER)
    {
        printf("CG is out of range.\n");
        printf("CG is too low.\n");
    }
    else
    {
        printf("CG is within the aircraft limits.\n");
    }
}

/*Function to get check the weight condition. Will check if the aircraft is overweight, underweight, or in balance.*/
void checkWeight(int totalWeight, int rampWeight, int fuel)
{
    double usefulLoadRemaining;
    /*Get the fuel weight at takeoff to calculate the takeoff weight.*/
    int fuelTO = 0.25 * FUELBURN; 			/*Taxi and runup approx. 15 minutes.*/
    int weightTO = rampWeight - (FUELWEIGHT * fuelTO);
    usefulLoadRemaining = usefulLoad - totalWeight;
    if (rampWeight >= MAXWEIGHT)
    {
        printf("Aircraft is overweight.\n");
    }
    else if (rampWeight <= MINWEIGHT)
    {
        printf("Aircraft is underweight.\n");
    }
    else
    {
        printf("Takeoff weight: %d\n", weightTO);
        printf("You have %.0lf lbs of useful load remaining.\n", usefulLoadRemaining);
    }
}

/*Function to generate and print the table*/
void generateTable(struct Weights weight, struct Arms arm, struct Moments moment, int rampWeight, int takeoffWeight, int CG) {
    clearScreen();
    printf("     WEIGHT AND BALANCE TABLE\n");
    printf("%34s\n", "==============================");
    printf("     Item | Weight | Arm | Moment\n");
    printf("%34s\n", "==============================");
    printf("     Pilot: %3d     %3d     %5d\n", weight.pilot, arm.pilotArm, moment.pilotMoment);
    printf("   CoPilot: %3d     %3d     %5d\n", weight.copilot, arm.copilotArm, moment.copilotMoment);
    printf("Passengers: %3d     %3d     %5d\n", weight.passengers, arm.passengersArm, moment.passMoment);
    printf("      Fuel: %3d     %3d     %5d\n", weight.fuel, arm.fuelArm, moment.fuelMoment);
    printf(" Baggage 1: %3d     %3d     %5d\n", weight.baggageArea1, arm.baggageArm1, moment.bagMoment1);
    printf(" Baggage 2: %3d     %3d     %5d\n", weight.baggageArea2, arm.baggageArm2, moment.bagMoment2);
    printf("     Empty: %3d    %3d     %5d\n", weight.empty, arm.emptyArm, moment.emptyMoment);
    printf("%34s\n", "==============================");
    printf("   RAMP WEIGHT  = %4d lbs\n"
           "            CG  = %4d in\n"
           "TAKEOFF WEIGHT  = %4d lbs\n", rampWeight, CG, takeoffWeight);
}

/*Function to clear the screen. This will work on Windows and Linux systems.*/
void clearScreen()
{
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}
