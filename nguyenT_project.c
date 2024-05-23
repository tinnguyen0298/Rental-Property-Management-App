/*
Name: Trung Tin Nguyen
Instructor: Deb Harding 
Course: CS2060-001
OS System: Windows 11
Summary: 
This program is designed for property owners to set up and rent their properties, 
with a pricing structure based on three intervals. 
Vacationers can rate the properties, 
and owners can end rentals and receive summary reports.
*/
/*include*/
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

/*define*/
//Rental property login and sentinal values
#define CORRECT_ID "id" 
#define CORRECT_PASSCODE "ab" 
#define LOGIN_MAX_ATTEMPTS 2
#define SENTINAL_NEG1 -1
//Maximum length of a string
#define STRING_LENGTH 80
//Two dimensional array storage amounts for rows and columns of surve data
#define VACATION_RENTERS 5
#define RENTER_SURVEY_CATEGORIES 4
//rental property constant ranges
#define DISCOUNT_MULTIPLIER 2
#define MIN_RATE 1
#define MAX_RATE 1000
#define MIN_RENTAL_NIGHTS 1
#define MAX_RENTAL_NIGHTS 14
#define MIN_RATINGS 1
#define MAX_RATINGS 5

/*struct*/
typedef struct {
	int property_Ratings[VACATION_RENTERS][RENTER_SURVEY_CATEGORIES]; 
	char property_Name[STRING_LENGTH];
	char property_Location[STRING_LENGTH];
	unsigned int rental_Rate;
	unsigned int discount_Rate;
	unsigned int interval_1;
	unsigned int interval_2;
	unsigned int total_Renters;
	unsigned int total_Nights;
	unsigned int total_Charge;
} Property;
/*prototype*/
bool getValidLogin(const char* id, const char* passcode, int attempts);
void trimNewLineChar(char* string);
void setupRentalProperty(int min_nights, int max_nights, int min_rate, int max_rate, Property* propertyPtr);
void rentalMode(Property* propertyPtr, int sentinel, const char* categories[]);
bool scanInt(const char* stringPtr, int* valuePtr);
bool checkEmptyRow(int index);
void getRatings(Property* propertyPtr, size_t surveyRow, size_t categoryColumn, const char* surveycategories[]);
void getRatingAverages(const int surveyRatings[][RENTER_SURVEY_CATEGORIES], double surveyAverages[], size_t numOfRenters, size_t numOfCategories);
double calculateCharge(unsigned int nights, unsigned int interval1Nights, unsigned int interval2Nights, double rate, double discount);
void printRentalPropertyInfo(unsigned int minNights, unsigned int maxNights, Property* propertyPtr, const char* categories[]);
void printNightsCharge(Property* propertyPtr);
void printCategoryData(const double categoryaverage[RENTER_SURVEY_CATEGORIES], size_t categories);
void printCategories(const char* categories[], size_t totalCategories);
void printSurveyResults(Property* propertyPtr, size_t surveyNumber, size_t categoryNumber, const char* categories[]);
/*main*/
int main(void)
{
	/*initialization*/
	Property property = { {0} }; // initialize all ratings to zero
	double ratingAverages[RENTER_SURVEY_CATEGORIES] = { 0 }; // initialize rating averages array
	const char* surveyCategories[RENTER_SURVEY_CATEGORIES] = { "Check-in Process", "Cleanliness\t", "Amenities\t", "cat4"};
	bool running = false;
	/*logic for the program*/
	puts("Welcome to AirUCCS!");
	running = getValidLogin(CORRECT_ID, CORRECT_PASSCODE, LOGIN_MAX_ATTEMPTS);
	if (running == true)
	{
		setupRentalProperty(MIN_RENTAL_NIGHTS, MAX_RENTAL_NIGHTS, MIN_RATE, MAX_RATE, &property);
		rentalMode(&property, SENTINAL_NEG1, surveyCategories);
		getRatingAverages(property.property_Ratings, ratingAverages, VACATION_RENTERS, RENTER_SURVEY_CATEGORIES);
		printNightsCharge(&property);
		printCategories(surveyCategories, RENTER_SURVEY_CATEGORIES);
		printCategoryData(ratingAverages, RENTER_SURVEY_CATEGORIES);
	}
	puts("\n\nExiting AirUCCS");
} // end main

/*user defined functions*/

/*
Function: getValidInt()
- Read in integer within a range of min to max or equal to sentinel value
Parameters:
- min: lowest valid value
- max: highest valid value
Return:
- A valid integer within the range or sentinal value
*/
int getValidInt(int min, int max)
{
	char userInput[STRING_LENGTH];
	int validInt = 0;
	bool isValid = false;

	while (isValid == false)
	{
		fgets(userInput, STRING_LENGTH, stdin);
		trimNewLineChar(userInput);
		isValid = scanInt(userInput, &validInt);
		if (validInt == SENTINAL_NEG1)
		{
			isValid = true;
		}
		// if user input is out of range, prompt to enter again
		else if (validInt < min || validInt > max)
		{
			if (min == MIN_RATINGS && max == MAX_RATINGS)
			{
				if (isValid == true)
				{
					printf("Enter value between %d and %d. ", MIN_RATINGS, MAX_RATINGS);
				}
				else
				{
					printf("Invalid Input. Try again.\n");
				}
			}
			else
			{
				printf("Invalid Input. Try again.\n");
			}
			isValid = false;
		}
	}
	return validInt;
}

/*
scanInt: validate user input
- stringPtr is a pointer to character array of user input
- valuePtr is a pointer to an integer variable
return:
- true if the user input is a valid integer
- false if the user input is something different from an integer
*/
bool scanInt(const char* stringPtr, int* valuePtr)
{
	char* end = NULL;
	errno = 0;
	bool result = false;
	long intTest = strtol(stringPtr, &end, 10);
	if (end == stringPtr)
	{
		//fprintf(stderr, "%s: not a decimal number\n", stringPtr);
	}
	else if ('\0' != *end)
	{
		//fprintf(stderr, "%s: extra charaters at the end of input: %s\n", stringPtr, end);
	}
	else if ((intTest == LONG_MIN || intTest == LONG_MAX) && ERANGE == errno)
	{
		fprintf(stderr, "%s: out of range of long integer\n", stringPtr);
	}
	else if (intTest > INT_MAX)
	{
		fprintf(stderr, "%ld: greater than INT_MAX\n", intTest);
	}
	else if (intTest < INT_MIN)
	{
		fprintf(stderr, "%ld: less than INT_MIN\n", intTest);
	}
	else
	{
		*valuePtr = (int)intTest;
		result = true;
	}
	return result;
}

/*
getValidLogIn: check if both ID and Passcode are correct
parameter:
- id is a pointer to a character array of ID
- passcode is a pointer to a character array of PASSCODE
- attempts is the maximum times of wrong input
return:
- true if both id and passcode are correct
- false if one of them is false
*/
bool getValidLogin(const char* id, const char* passcode, int attempts)
{
	bool validLogIn = false;
	int counter = 0;
	char inputID[STRING_LENGTH];
	char inputPasscode[STRING_LENGTH];
	puts("\nAirUCCS Rental Property Owner Login");
	while (validLogIn == false && counter < attempts)
	{
		puts("Enter your AirUCCS ID: ");
		fgets(inputID, STRING_LENGTH, stdin);
		trimNewLineChar(inputID);
		puts("Enter your AirUCCS Passcode: ");
		fgets(inputPasscode, STRING_LENGTH, stdin);
		trimNewLineChar(inputPasscode);
		// if both ID and Passcode are correct, return true
		if ((strcmp(id, inputID)) == 0 && (strcmp(passcode, inputPasscode)) == 0)
		{
			validLogIn = true;
		}
		else
		{
			puts("\nError: Invalid ID or Passcode.");
			counter++;
		}
	}
	return validLogIn;
}// end getValidLogIn

/*
trimNewLineChar: trim new line character at the end of a input string if it is exist
parameter:
- pointer to the string
return: 
- the last character of the string is replaced by a null character
*/
void trimNewLineChar(char* string)
{
	// if last character is new line character
	// replace it by a null character
	if (string[strlen(string) - 1] == '\n')
	{
		string[strlen(string) - 1] = '\0';
	}
}

/*
setupRentalProperty: prompt property's owner to input desired value for each rental information
parameters:
- min_nights: the minimum night to rent the property
- max_nights: the maximum night to rent the property
- min_rate: the minimum rental rate for the property
- max_rate: the maximum rental rate for the property
- property: pointer to Property structure
return:
- desired values for minimum and maximum rental rate, and discount rate
- desired night intervals 
*/
void setupRentalProperty(int min_nights, int max_nights, int min_rate, int max_rate, Property* propertyPtr)
{
	printf("Minimum nights is %d nights.\n", MIN_RENTAL_NIGHTS);
	printf("Max nights is %d nights.\n", MAX_RENTAL_NIGHTS);
	printf("Minimum rate is $%d\n", MIN_RATE);
	printf("Max rate is $%d\n", MAX_RATE);

	puts("Set up rental property information.");
	puts("Enter number of nights until first discount: ");
	propertyPtr->interval_1 = getValidInt(MIN_RENTAL_NIGHTS, MAX_RENTAL_NIGHTS);
	puts("Enter number of nights until second discount: ");
	propertyPtr->interval_2 = getValidInt(propertyPtr->interval_1 + 1, MAX_RENTAL_NIGHTS);
	puts("Enter the nightly rental rate: ");
	propertyPtr->rental_Rate = getValidInt(MIN_RATE, MAX_RATE);
	puts("Enter the discount: ");
	propertyPtr->discount_Rate = getValidInt(MIN_RATE, propertyPtr->rental_Rate);
	puts("Enter rental name: ");
	fgets(propertyPtr->property_Name, STRING_LENGTH, stdin);
	puts("Enter rental location: ");
	fgets(propertyPtr->property_Location, STRING_LENGTH, stdin);

}

/*
rentalMode: propmt renter to enter desired number of nights to stay at the property
parameters:
- propertyPtr: pointer to Property structure
- sentinal: flag value 
- categories: array of categories' name
return:

*/
void rentalMode(Property* propertyPtr, int sentinal, const char* categories[])
{
	bool exitRentalMode = false;
	unsigned int nights = 0;
	unsigned int charge = 0;
	printRentalPropertyInfo(MIN_RENTAL_NIGHTS, MAX_RENTAL_NIGHTS, propertyPtr, categories); 
	while (!exitRentalMode)
	{
		puts("\n\nEnter the number of nights you want to rent the property: ");
		nights = getValidInt(MIN_RENTAL_NIGHTS, MAX_RENTAL_NIGHTS);
		if (nights == sentinal)
		{
			//if login is successful, exit while loop
			//else return to while loop
			exitRentalMode = getValidLogin(CORRECT_ID, CORRECT_PASSCODE, LOGIN_MAX_ATTEMPTS);
		}
		else
		{
			charge = calculateCharge(nights, propertyPtr->interval_1, propertyPtr->interval_2, propertyPtr->rental_Rate, propertyPtr->discount_Rate);
			printf("Nights: %d \t\tCharge: $%d \n", nights, charge);
			// increment totals
			propertyPtr->total_Nights += nights;
			propertyPtr->total_Charge += charge;
			propertyPtr->total_Renters += 1;
			
			getRatings(propertyPtr, VACATION_RENTERS, RENTER_SURVEY_CATEGORIES, categories);
			printSurveyResults(propertyPtr, VACATION_RENTERS, RENTER_SURVEY_CATEGORIES, categories);
		}
	}
}

/*
getRatings: prompt renter to input rating for each category
parameters:
- property: pointer to Property structure
- surveyRow: maximum renters to give rating
- categoryColumn: maximum categories
return:
- fill ratings into rating array
- show "Array is full" message if there is no more room for rating
*/
void getRatings(Property* propertyPtr, size_t surveyRow, size_t categoryColumn, const char* surveycategories[])
{
	bool doneRating = false;
	size_t row = 0;
	puts("We want to know how your experience was renting our property.\n"
		"Using the rating system 1 to 5 enter your rating for each category:");
	printCategories(surveycategories, RENTER_SURVEY_CATEGORIES);
	while (row < surveyRow && doneRating == false)
	{
		//check if the array has a room for more survey
		bool emptyRow = checkEmptyRow(propertyPtr->property_Ratings[row][0]);

		if (emptyRow == true)
		{
			for (size_t column = 0; column < categoryColumn; ++column)
			{
				//puts("Enter your rating for ");
				printf("%zu. Enter rating for %s: ", column + 1, surveycategories[column]);
				//validate rating value and fill in the array 
				propertyPtr->property_Ratings[row][column] = getValidInt(MIN_RATINGS, MAX_RATINGS);
			}
			doneRating = true;
		}
		else if (emptyRow == false)
		{
			//move to next row of the array 
			row++;
			// if last row is not available, the array is full
			if (row == surveyRow)
			{
				puts("Rating array is full.");
			}
		}

	}
}

/*
checkEmptyRow: check if an array has a row that does not contain rating values 
parameter:
- index: first index of the row
return: 
- true if row is empty 
- false if row is not empty
*/
bool checkEmptyRow(int element)
{
	bool isEmpty = true;
	//check if an element at a index is any value of 1 to 5
	for (int i = 1; i < 6; ++i)
	{
		if (element == i)
		{
			isEmpty = false;
		}
	}
	return isEmpty;
}


/*
getRatingAverages: calculate rating average for each category and put the result into surveyAverages array
parameters:
- surveyRatings
- surveyAverages
- numOfRenters
- numOfCategories
return: 
- fill average into surveyAverages array
*/
void getRatingAverages(const int surveyRatings[][RENTER_SURVEY_CATEGORIES], double surveyAverages[], size_t totalRenters, size_t totalCategories)
{
	for (size_t i = 0; i < totalCategories; ++i)
	{
		double total = 0.0;
		double average = 0.0;
		int renters = totalRenters;
		for (size_t j = 0; j < totalRenters; ++j)
		{
			total += surveyRatings[j][i];
			if (surveyRatings[j][0] == 0)
			{
				renters = renters - 1;
			}
		}
		average = (double)(total / renters); 
		surveyAverages[i] = average;
	}
}

/*
calculateCharges: calculates the charge based on the number of nights rented
parameters:
- nights: number of nights that are used to calculate charge
- interval1Nights: first interval of nights
- interval2Nights: second interval of nigths
- rate: regular rental rate
- discount: discount rate
return:
- charge value
*/
double calculateCharge(unsigned int nights, unsigned int interval1Nights,
	unsigned int interval2Nights, double rate, double discount)
{
	double charge = 0;
	// calculate charge for the first 3 nights 
	if (nights <= interval1Nights)
	{
		charge = nights * rate;
	}
	// calculate charge for nights 4 to 6
	else if (nights > interval1Nights && nights <= interval2Nights)
	{
		charge = (nights * rate) - ((nights - interval1Nights) * discount);
	}
	// calculate charge for nigths over 6
	else
	{
		charge = (nights * rate) - ((interval2Nights - interval1Nights) * discount)
			- ((nights - interval2Nights) * discount * DISCOUNT_MULTIPLIER);
	}
	return charge;
}

/*
printRentalPropertyInfo: print out information of the renntal property
parameters:
- minNights: minimum number of nights to rent the property
- maxNights: maximum number of nights to rent the property
- interval1Nights: first night interval
- interval2Nights: second night interval
- rate: regular rental rate
- discount: discount rate
return:
- rental property information
*/
void printRentalPropertyInfo(unsigned int minNights, unsigned int maxNights, Property* propertyPtr, const char* categories[])
{
	// print rental property information 
	puts("*********************#*#*********************");
	printf("Rental Property: %s", propertyPtr->property_Name);
	printf("Location: %s", propertyPtr->property_Location);
	printf("Rental Property can be rented for %d to %d nights.", minNights, maxNights);
	printf("\n$%d rate a night for the first %d nights", propertyPtr->rental_Rate, propertyPtr->interval_1);
	printf("\n$%d discount rate a night for nights %d to %d", propertyPtr->discount_Rate, propertyPtr->interval_1, propertyPtr->interval_2);
	printf("\n$%d discount rate a nigth for each remaining night over %d", propertyPtr->discount_Rate * DISCOUNT_MULTIPLIER, propertyPtr->interval_2);
	printSurveyResults(propertyPtr, VACATION_RENTERS, RENTER_SURVEY_CATEGORIES, categories);
}

/*
printNightsCharges: print out total nights and total charge 
parameters:
- nights: desired number of nights
- charge: total charge based on number of nights
return:
- total nights
- total charge 
*/
void printNightsCharge(Property* propertyPtr)
{
	puts("-----------$*$*$-----------");
	puts("Rental Property Report");
	printf("Name: %s", propertyPtr->property_Name);
	printf("Location: %s", propertyPtr->property_Location);
	printf("%s", "\nRental Property Totals");
	if (propertyPtr->total_Nights != 0) {
		printf("\nRenters\t\tNights\t\tCharge\n%d\t\t%d\t\t$%d\n", propertyPtr->total_Renters, propertyPtr->total_Nights, propertyPtr->total_Charge);
	}
	else
	{
		printf("\n\nThere were no rentals.\n\n");
	}
}

/*
printCategories: print out name of each category 
parameters:
- categories: array of names
- totalCategories: number of categories in the array
return:
- print out categories' names
*/
void printCategories(const char* categories[], size_t totalCategories)
{
	//loop to display each category horizontally
	printf("%s", "Rating Categories:\t");
	for (size_t surveyCategory = 0; surveyCategory < totalCategories; ++surveyCategory)
	{
		printf("\t%zu.%s  ", surveyCategory + 1, categories[surveyCategory]);
	}
	puts(""); // start new line of output
}

/*
printCategoryData: print out rating average for each category
parameters:
- categoryaverage: ratign average array
- categories: number of categories or column in the array
return:
- average values for each category 
*/
void printCategoryData(const double categoryaverage[RENTER_SURVEY_CATEGORIES], size_t categories)
{
	printf("%s", "Rating Averages\t\t\t");
	// interate and print out average values of each categories
	for (size_t column = 0; column < categories; ++column)
	{
		printf("%-5.2lf\t\t\t", categoryaverage[column]);
	}

}

/*
printSurveyResult: print out result of survey 
parameters:
- proprtyPtr: pointer to Property structure
- surveyNumber: total survey 
- categoryNumber: total category
- categories: array of categries' names
return: 
- print out result of each survey  
*/
void printSurveyResults(Property* propertyPtr, size_t surveyNumber, size_t categoryNumber, const char* categories[])
{
	//puts("\nSurvey Results");
	bool noRatings = false; 
	// check first index of the array
	noRatings = checkEmptyRow(propertyPtr->property_Ratings[0][0]);
	if (noRatings == true)
	{
		puts("\nNo Rating Currently");
	}
	else
	{
		//printCategories(categories, RENTER_SURVEY_CATEGORIES);
		// iterate each survey and print out results of one survey at a time
		for (size_t row = 0; row < surveyNumber; ++row)
		{
			if (propertyPtr->property_Ratings[row][1] != 0)
			{
				printf("\nSurvey %zu:\t", row + 1);
				for (size_t column = 0; column < categoryNumber; ++column)
				{
					printf("%-5d\t", propertyPtr->property_Ratings[row][column]);
				}
			}
		}
	}
}